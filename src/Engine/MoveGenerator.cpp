#include "Engine/MoveGenerator.h"


MoveGenerator::MoveGenerator(Board& board) :
	m_board{board},
	m_magicBitboardHelper{}
{}

MoveGenerationContext MoveGenerator::GetMoveGenerationContext() const {
	const Bitboard* const pBitboards = m_board.GetPieceBitboards();

	const Bitboard* pFriendlyBitboards;
	const Bitboard* pEnemyBitboards;

	if (m_board.IsWhiteTurn()) {
		pFriendlyBitboards = pBitboards;
		pEnemyBitboards = pBitboards + 6;
	} else {
		pFriendlyBitboards = pBitboards + 6;
		pEnemyBitboards = pBitboards;
	}

	Bitboard friendlyPawnBB = *(pFriendlyBitboards);
	Bitboard friendlyKnightBB = *(pFriendlyBitboards + 1);
	Bitboard friendlyBishopBB = *(pFriendlyBitboards + 2);
	Bitboard friendlyRookBB = *(pFriendlyBitboards + 3);
	Bitboard friendlyQueenBB = *(pFriendlyBitboards + 4);
	Bitboard friendlyKingBB = *(pFriendlyBitboards + 5);

	Bitboard enemyPawnBB = *(pEnemyBitboards);
	Bitboard enemyKnightBB = *(pEnemyBitboards + 1);
	Bitboard enemyBishopBB = *(pEnemyBitboards + 2);
	Bitboard enemyRookBB = *(pEnemyBitboards + 3);
	Bitboard enemyQueenBB = *(pEnemyBitboards + 4);
	Bitboard enemyKingBB = *(pEnemyBitboards + 5);

	Bitboard friendlyPieceBB =
		friendlyPawnBB 		|
		friendlyKnightBB 	|
		friendlyBishopBB 	|
		friendlyRookBB 		|
		friendlyQueenBB 	|
		friendlyKingBB;

	Bitboard enemyPieceBB =
		enemyPawnBB 	|
		enemyKnightBB 	|
		enemyBishopBB 	|
		enemyRookBB 	|
		enemyQueenBB 	|
		enemyKingBB;
	
	Square enPassantSquare = m_board.GetEnPassantSquare();

	Bitboard allPieceBB = friendlyPieceBB | enemyPieceBB;
	Bitboard emptySquareBB = ~allPieceBB;

	Bitboard enemyOrthogonalPieceBB = enemyRookBB | enemyQueenBB;
	Bitboard enemyDiagonalPieceBB = enemyBishopBB | enemyQueenBB;

	Bitboard noKingBB = allPieceBB & ~friendlyKingBB;

	Bitboard enemyAttackSetBB = GetAttackSet(enemyPawnBB, enemyKnightBB, enemyBishopBB, enemyRookBB, enemyQueenBB, enemyKingBB, emptySquareBB, noKingBB);

	Square friendlyKingSquare = static_cast<Square>(friendlyKingBB);

	Bitboard checkerBB{0ULL};

	Bitboard kingPawnAttackSet = m_board.IsWhiteTurn() ? m_magicBitboardHelper.GetWhitePawnAttacks(friendlyKingSquare) : m_magicBitboardHelper.GetBlackPawnAttacks(friendlyKingSquare);
	checkerBB |= (kingPawnAttackSet & enemyPawnBB);

	Bitboard kingKnightAttackSet = m_magicBitboardHelper.GetKnightAttacks(friendlyKingSquare);
	checkerBB |= (kingKnightAttackSet & enemyKnightBB);

	Bitboard orthogonalOccupancy = GetOrthogonalOccupancyMask(friendlyKingSquare) & allPieceBB;
	Bitboard kingOrthogonalAttackSet = m_magicBitboardHelper.GetOrthogonalAttacks(friendlyKingSquare, orthogonalOccupancy);
	checkerBB |= (kingOrthogonalAttackSet & enemyOrthogonalPieceBB);

	Bitboard diagonalOccupancy = GetDiagonalOccupancyMask(friendlyKingSquare) & allPieceBB;
	Bitboard kingDiagonalAttackSet = m_magicBitboardHelper.GetDiagonalAttacks(friendlyKingSquare, diagonalOccupancy);
	checkerBB |= (kingDiagonalAttackSet & enemyDiagonalPieceBB);

	Bitboard checkMaskBB{FULL_BOARD};

	std::array<Bitboard, static_cast<size_t>(Square::COUNT)> pinMasks;
	pinMasks.fill(FULL_BOARD);

	MoveGenerationContext context {
		friendlyPawnBB,
		friendlyKnightBB,
		friendlyBishopBB,
		friendlyRookBB,
		friendlyQueenBB,
		friendlyKingBB,
		friendlyKingSquare,
		friendlyPieceBB,
		enemyPawnBB,
		enemyKnightBB,
		enemyBishopBB,
		enemyRookBB,
		enemyQueenBB,
		enemyKingBB,
		enPassantSquare,
		allPieceBB,
		emptySquareBB,
		enemyPieceBB,
		enemyAttackSetBB,
		checkMaskBB,
		pinMasks,
		checkerBB
	};

	return context;
}

bool MoveGenerator::IsCheck() const {
	const MoveGenerationContext context = GetMoveGenerationContext();
	return IsCheck(context);
}

bool MoveGenerator::IsCheck(const MoveGenerationContext& context) const {
	return context.m_checkerBB.PopCount() != 0;
}

bool MoveGenerator::GenerateMoves(const MoveGenerationParameters& params) const {
	MoveGenerationContext context = GetMoveGenerationContext();

	return GenerateMoves(params, context);
}

bool MoveGenerator::GenerateMoves(const MoveGenerationParameters& params, MoveGenerationContext& context) const {
	params.m_moves.clear();

	size_t numCheckers = context.m_checkerBB.PopCount();
	bool inCheck = numCheckers != 0;

	if (numCheckers == 2) {
		// If there are two checkers then great. We have sufficient context by this point
		GenerateKingMoves(params, context);
		return inCheck;
	}

	if (numCheckers == 1) {
		Square checkerSquare = static_cast<Square>(context.m_checkerBB);
		context.m_checkMaskBB = m_magicBitboardHelper.GetBetweenMask(context.m_friendlyKingSquare, checkerSquare) | context.m_checkerBB;
	}

	// Work out pin masks by getting king ray masks, ANDing them with relevant pieces, getting between masks.
	// If the follow set contains just one piece and it's one of ours, then the mask of allowed squares for that piece is the between mask.
	Bitboard kingOrthogonalPotentialAttackers = m_magicBitboardHelper.GetOrthogonalRays(context.m_friendlyKingSquare) & (context.m_enemyRookBB | context.m_enemyQueenBB);
	Bitboard kingDiagonalPotentialAttackers = m_magicBitboardHelper.GetDiagonalRays(context.m_friendlyKingSquare) & (context.m_enemyBishopBB | context.m_enemyQueenBB);

	Bitboard kingPotentialAttackers = kingOrthogonalPotentialAttackers | kingDiagonalPotentialAttackers;

	for (Square potentialAttacker : kingPotentialAttackers) {
		Bitboard pinBB = m_magicBitboardHelper.GetBetweenMask(context.m_friendlyKingSquare, potentialAttacker);
		Bitboard blockerBB = pinBB & context.m_allPieceBB;

		size_t numBlockers = blockerBB.PopCount();
		if (numBlockers == 1 && (blockerBB & context.m_friendlyPieceBB).Any()) {
			Square blocker = static_cast<Square>(blockerBB);
			context.m_pinMasks[static_cast<size_t>(blocker)] &= pinBB | Bitboard{potentialAttacker};
		}
	}

	GeneratePawnMoves(params, context);
	GenerateKnightMoves(params, context);
	GenerateBishopMoves(params, context);
	GenerateRookMoves(params, context);
	GenerateQueenMoves(params, context);
	GenerateKingMoves(params, context);

	if (numCheckers == 0)
		GenerateCastleMoves(params, context);

	return inCheck;
}

inline void MoveGenerator::GeneratePawnMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	if (m_board.IsWhiteTurn()) {
		GenerateWhitePawnMoves(params, context);
	} else {
		GenerateBlackPawnMoves(params, context);
	}
}

inline void MoveGenerator::GenerateWhitePawnMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	for (Square pawnSquare : context.m_friendlyPawnBB) {
		Bitboard pawnBB{pawnSquare};

		Bitboard attackSetBB = m_magicBitboardHelper.GetWhitePawnAttacks(pawnSquare);

		// En passant
		Bitboard enPassantBB = attackSetBB & Bitboard{context.m_enPassantSquare};
		if (context.m_enPassantSquare != Square::NONE && enPassantBB.Any()) {
			// Can we just use XOR below?
			Bitboard removedPawnBB = context.m_allPieceBB & ~pawnBB & ~enPassantBB.ShiftSouth() | enPassantBB;

			Bitboard enemyOrthogonalsBB = context.m_enemyRookBB | context.m_enemyQueenBB;
			Bitboard orthogonalOccupancy = GetOrthogonalOccupancyMask(context.m_friendlyKingSquare) & removedPawnBB;
			Bitboard orthogonalAttackers = m_magicBitboardHelper.GetOrthogonalAttacks(context.m_friendlyKingSquare, orthogonalOccupancy) & enemyOrthogonalsBB;

			if (orthogonalAttackers.Empty()) {
				Bitboard enemyDiagonalsBB = context.m_enemyBishopBB | context.m_enemyQueenBB;
				Bitboard diagonalOccupancy = GetDiagonalOccupancyMask(context.m_friendlyKingSquare) & removedPawnBB;
				Bitboard diagonalAttackers = m_magicBitboardHelper.GetDiagonalAttacks(context.m_friendlyKingSquare, diagonalOccupancy) & enemyDiagonalsBB;

				if (diagonalAttackers.Empty()) {
					int mvv_lva = ABSOLUTE_PIECE_VALUES[Piece::BLACK_PAWN] * 10 - ABSOLUTE_PIECE_VALUES[Piece::WHITE_PAWN];
					params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, context.m_enPassantSquare, Piece::EMPTY, false, false, true, false});
				}
			}
		}

		// Captures
		Bitboard captureBB = attackSetBB & context.m_enemyPieceBB & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(pawnSquare)];
		if ((pawnBB & RANK_7).Any()) {
			for (Square to : captureBB) {
				Piece victim = m_board.GetPieceAtSquare(to);
				int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[Piece::WHITE_PAWN];

				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::WHITE_KNIGHT, true, false, false, false});
				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::WHITE_BISHOP, true, false, false, false});
				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::WHITE_ROOK, true, false, false, false});
				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::WHITE_QUEEN, true, false, false, false});
			}
		} else {
			for (Square to : captureBB) {
				Piece victim = m_board.GetPieceAtSquare(to);
				int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[Piece::WHITE_PAWN];

				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::EMPTY, true, false, false, false});
			}
		}

		if (params.m_capturesOnly)
			continue;

		// Quiet moves
		Bitboard pawnPushBB = pawnBB.ShiftNorth() & context.m_emptySquareBB;

		if (pawnPushBB.Empty())
			continue;

		Bitboard pawnPushAllowedBB = pawnPushBB & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(pawnSquare)];

		if (pawnPushAllowedBB.Any()) {
			Square pawnPushSquare = static_cast<Square>(pawnPushBB);
			if ((pawnPushBB & RANK_8).Any()) {
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_KNIGHT], pawnSquare, pawnPushSquare, Piece::WHITE_KNIGHT, false, false, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_BISHOP], pawnSquare, pawnPushSquare, Piece::WHITE_BISHOP, false, false, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_ROOK], pawnSquare, pawnPushSquare, Piece::WHITE_ROOK, false, false, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_QUEEN], pawnSquare, pawnPushSquare, Piece::WHITE_QUEEN, false, false, false, false});
				continue;
			} else {
				params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, pawnSquare, pawnPushSquare, Piece::EMPTY, false, false, false, false});
			}
		}

		if ((pawnBB & RANK_2).Empty())
			continue;

		Bitboard pawnPushPushBB = pawnPushBB.ShiftNorth() & context.m_emptySquareBB;
		
		if (pawnPushPushBB.Empty())
			continue;

		Bitboard pawnPushPushAllowedBB = pawnPushPushBB & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(pawnSquare)];
		
		if (pawnPushPushAllowedBB.Any()) {
			Square pawnPushPushSquare = static_cast<Square>(pawnPushPushBB);

			if ((pawnPushPushBB & RANK_8).Any()) {
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_KNIGHT], pawnSquare, pawnPushPushSquare, Piece::WHITE_KNIGHT, false, true, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_BISHOP], pawnSquare, pawnPushPushSquare, Piece::WHITE_BISHOP, false, true, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_ROOK], pawnSquare, pawnPushPushSquare, Piece::WHITE_ROOK, false, true, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::WHITE_QUEEN], pawnSquare, pawnPushPushSquare, Piece::WHITE_QUEEN, false, true, false, false});
			} else {
				params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, pawnSquare, pawnPushPushSquare, Piece::EMPTY, false, true, false, false});
			}
		}
	}
}

inline void MoveGenerator::GenerateBlackPawnMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	for (Square pawnSquare : context.m_friendlyPawnBB) {
		Bitboard pawnBB{pawnSquare};

		Bitboard attackSetBB = m_magicBitboardHelper.GetBlackPawnAttacks(pawnSquare);

		// En passant
		Bitboard enPassantBB = attackSetBB & Bitboard{context.m_enPassantSquare};
		if (context.m_enPassantSquare != Square::NONE && enPassantBB.Any()) {
			// Can we just use XOR below?
			Bitboard removedPawnBB = context.m_allPieceBB & ~pawnBB & ~enPassantBB.ShiftNorth() | enPassantBB;

			Bitboard enemyOrthogonalsBB = context.m_enemyRookBB | context.m_enemyQueenBB;
			Bitboard orthogonalOccupancy = GetOrthogonalOccupancyMask(context.m_friendlyKingSquare) & removedPawnBB;
			Bitboard orthogonalAttackers = m_magicBitboardHelper.GetOrthogonalAttacks(context.m_friendlyKingSquare, orthogonalOccupancy) & enemyOrthogonalsBB;

			if (orthogonalAttackers.Empty()) {
				Bitboard enemyDiagonalsBB = context.m_enemyBishopBB | context.m_enemyQueenBB;
				Bitboard diagonalOccupancy = GetDiagonalOccupancyMask(context.m_friendlyKingSquare) & removedPawnBB;
				Bitboard diagonalAttackers = m_magicBitboardHelper.GetDiagonalAttacks(context.m_friendlyKingSquare, diagonalOccupancy) & enemyDiagonalsBB;

				if (diagonalAttackers.Empty()) {
					int mvv_lva = ABSOLUTE_PIECE_VALUES[Piece::WHITE_PAWN] * 10 - ABSOLUTE_PIECE_VALUES[Piece::BLACK_PAWN];
					params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, context.m_enPassantSquare, Piece::EMPTY, false, false, true, false});
				}
			}
		}

		// Captures
		Bitboard captureBB = attackSetBB & context.m_enemyPieceBB & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(pawnSquare)];
		if ((pawnBB & RANK_2).Any()) {
			for (Square to : captureBB) {
				Piece victim = m_board.GetPieceAtSquare(to);
				int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[Piece::BLACK_PAWN];

				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::BLACK_KNIGHT, true, false, false, false});
				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::BLACK_BISHOP, true, false, false, false});
				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::BLACK_ROOK, true, false, false, false});
				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::BLACK_QUEEN, true, false, false, false});
			}
		} else {
			for (Square to : captureBB) {
				Piece victim = m_board.GetPieceAtSquare(to);
				int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[Piece::BLACK_PAWN];

				params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, pawnSquare, to, Piece::EMPTY, true, false, false, false});
			}
		}

		if (params.m_capturesOnly)
			continue;

		// Quiet moves
		Bitboard pawnPushBB = pawnBB.ShiftSouth() & context.m_emptySquareBB;

		Bitboard pawnPushAllowedBB = pawnPushBB & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(pawnSquare)];

		if (pawnPushAllowedBB.Any()) {
			Square pawnPushSquare = static_cast<Square>(pawnPushBB);
			if ((pawnPushBB & RANK_1).Any()) {
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_KNIGHT], pawnSquare, pawnPushSquare, Piece::BLACK_KNIGHT, false, false, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_BISHOP], pawnSquare, pawnPushSquare, Piece::BLACK_BISHOP, false, false, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_ROOK], pawnSquare, pawnPushSquare, Piece::BLACK_ROOK, false, false, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_QUEEN], pawnSquare, pawnPushSquare, Piece::BLACK_QUEEN, false, false, false, false});
				continue;
			} else {
				params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, pawnSquare, pawnPushSquare, Piece::EMPTY, false, false, false, false});
			}
		}

		if ((pawnBB & RANK_7).Empty())
			continue;

		Bitboard pawnPushPushBB = pawnPushBB.ShiftSouth() & context.m_emptySquareBB;

		if (pawnPushPushBB.Empty())
			continue;

		Bitboard pawnPushPushAllowedBB = pawnPushPushBB & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(pawnSquare)];

		if (pawnPushPushAllowedBB.Any()) {
			Square pawnPushPushSquare = static_cast<Square>(pawnPushPushBB);
			if ((pawnPushPushBB & RANK_1).Any()) {
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_KNIGHT], pawnSquare, pawnPushPushSquare, Piece::BLACK_KNIGHT, false, true, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_BISHOP], pawnSquare, pawnPushPushSquare, Piece::BLACK_BISHOP, false, true, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_ROOK], pawnSquare, pawnPushPushSquare, Piece::BLACK_ROOK, false, true, false, false});
				params.m_moves.push_back(Move{PROMOTION_BASE_SCORE + ABSOLUTE_PIECE_VALUES[Piece::BLACK_QUEEN], pawnSquare, pawnPushPushSquare, Piece::BLACK_QUEEN, false, true, false, false});
			} else {
				params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, pawnSquare, pawnPushPushSquare, Piece::EMPTY, false, true, false, false});
			}
		}
	}
}

inline void MoveGenerator::GenerateKnightMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	for (Square knightSquare : context.m_friendlyKnightBB) {
		if (knightSquare == Square::NONE) continue;

		Bitboard attackSet = m_magicBitboardHelper.GetKnightAttacks(knightSquare);

		Bitboard possibleMoveBB = attackSet & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(knightSquare)];

		Bitboard captureBB = possibleMoveBB & context.m_enemyPieceBB;
		for (Square to : captureBB) {
			Piece victim = m_board.GetPieceAtSquare(to);
			int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[WHITE_KNIGHT];

			params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, knightSquare, to, Piece::EMPTY, true, false, false, false});
		}

		if (params.m_capturesOnly)
			continue;

		Bitboard quietMoveBB = possibleMoveBB & context.m_emptySquareBB;
		for (Square to : quietMoveBB) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, knightSquare, to, Piece::EMPTY, false, false, false, false});
		}
	}
}

inline void MoveGenerator::GenerateBishopMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	for (Square bishopSquare : context.m_friendlyBishopBB) {
		Bitboard occupancyMask = GetDiagonalOccupancyMask(bishopSquare);
		Bitboard occupancy = occupancyMask & context.m_allPieceBB;

		Bitboard attackSet = m_magicBitboardHelper.GetDiagonalAttacks(bishopSquare, occupancy);

		Bitboard possibleMoveBB = attackSet & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(bishopSquare)];

		Bitboard captureBB = possibleMoveBB & context.m_enemyPieceBB;
		for (Square to : captureBB) {
			Piece victim = m_board.GetPieceAtSquare(to);
			int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[WHITE_BISHOP];

			params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, bishopSquare, to, Piece::EMPTY, true, false, false, false});
		}

		if (params.m_capturesOnly)
			continue;

		Bitboard quietMoveBB = possibleMoveBB & context.m_emptySquareBB;
		for (Square to : quietMoveBB) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, bishopSquare, to, Piece::EMPTY, false, false, false, false});
		}
	}
}

inline void MoveGenerator::GenerateRookMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	for (Square rookSquare : context.m_friendlyRookBB) {
		Bitboard occupancyMask = GetOrthogonalOccupancyMask(rookSquare);
		Bitboard occupancy = occupancyMask & context.m_allPieceBB;

		Bitboard attackSet = m_magicBitboardHelper.GetOrthogonalAttacks(rookSquare, occupancy);

		Bitboard possibleMoveBB = attackSet & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(rookSquare)];

		Bitboard captureBB = possibleMoveBB & context.m_enemyPieceBB;
		for (Square to : captureBB) {
			Piece victim = m_board.GetPieceAtSquare(to);
			int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[WHITE_ROOK];

			params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, rookSquare, to, Piece::EMPTY, true, false, false, false});
		}

		if (params.m_capturesOnly)
			continue;

		Bitboard quietMoveBB = possibleMoveBB & context.m_emptySquareBB;
		for (Square to : quietMoveBB) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, rookSquare, to, Piece::EMPTY, false, false, false, false});
		}
	}
}

inline void MoveGenerator::GenerateQueenMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	for (Square queenSquare : context.m_friendlyQueenBB) {

		Bitboard orthogonalOccupancyMask = GetOrthogonalOccupancyMask(queenSquare);
		Bitboard orthogonalOccupancy = orthogonalOccupancyMask & context.m_allPieceBB;
		Bitboard orthogonalAttackSet = m_magicBitboardHelper.GetOrthogonalAttacks(queenSquare, orthogonalOccupancy);

		Bitboard diagonalOccupancyMask = GetDiagonalOccupancyMask(queenSquare);
		Bitboard diagonalOccupancy = diagonalOccupancyMask & context.m_allPieceBB;
		Bitboard diagonalAttackSet = m_magicBitboardHelper.GetDiagonalAttacks(queenSquare, diagonalOccupancy);

		Bitboard attackSet = orthogonalAttackSet | diagonalAttackSet;

		Bitboard possibleMoveBB = attackSet & context.m_checkMaskBB & context.m_pinMasks[static_cast<size_t>(queenSquare)];

		Bitboard captureBB = possibleMoveBB & context.m_enemyPieceBB;
		for (Square to : captureBB) {
			Piece victim = m_board.GetPieceAtSquare(to);
			int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[WHITE_QUEEN];

			params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, queenSquare, to, Piece::EMPTY, true, false, false, false});
		}

		if (params.m_capturesOnly)
			continue;

		Bitboard quietMoveBB = possibleMoveBB & context.m_emptySquareBB;
		for (Square to : quietMoveBB) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, queenSquare, to, Piece::EMPTY, false, false, false, false});
		}
	}
}

inline void MoveGenerator::GenerateKingMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	Bitboard attackSetBB = m_magicBitboardHelper.GetKingAttacks(context.m_friendlyKingSquare);

	Bitboard possibleMoveBB = attackSetBB & ~context.m_enemyAttackSet;

	Bitboard captures = possibleMoveBB & context.m_enemyPieceBB;
	for (Square to : captures) {
		Piece victim = m_board.GetPieceAtSquare(to);
		int mvv_lva = ABSOLUTE_PIECE_VALUES[victim] * 10 - ABSOLUTE_PIECE_VALUES[WHITE_KING];
		params.m_moves.push_back(Move{CAPTURE_BASE_SCORE + mvv_lva, context.m_friendlyKingSquare, to, Piece::EMPTY, true, false, false, false});
	}

	if (params.m_capturesOnly)
			return;
		
	Bitboard quietMoves = possibleMoveBB & context.m_emptySquareBB;
	for (Square to : quietMoves) {
		params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, context.m_friendlyKingSquare, to, Piece::EMPTY, false, false, false, false});
	}

}

inline void MoveGenerator::GenerateCastleMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	if (params.m_capturesOnly)
		return;

	if (m_board.IsWhiteTurn())
		GenerateWhiteCastleMoves(params, context);
	else
		GenerateBlackCastleMoves(params, context);
}

void MoveGenerator::GenerateWhiteCastleMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	if (m_board.GetCastlePermission(CastlePermission::WHITE_KINGSIDE)) {
		bool isKingsideClear = (context.m_allPieceBB & WHITE_KINGSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (context.m_enemyAttackSet & WHITE_KINGSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isKingsideClear && isNotThroughCheck) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, context.m_friendlyKingSquare, Square::g1, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.GetCastlePermission(CastlePermission::WHITE_QUEENSIDE)) {
		bool isQueensideClear = (context.m_allPieceBB & WHITE_QUEENSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (context.m_enemyAttackSet & WHITE_QUEENSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isQueensideClear && isNotThroughCheck) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, context.m_friendlyKingSquare, Square::c1, Piece::EMPTY, false, false, false, true});
		}
	}
}

void MoveGenerator::GenerateBlackCastleMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const {
	if (m_board.GetCastlePermission(CastlePermission::BLACK_KINGSIDE)) {
		bool isKingsideClear = (context.m_allPieceBB & BLACK_KINGSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (context.m_enemyAttackSet & BLACK_KINGSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isKingsideClear && isNotThroughCheck) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, context.m_friendlyKingSquare, Square::g8, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.GetCastlePermission(CastlePermission::BLACK_QUEENSIDE)) {
		bool isQueensideClear = (context.m_allPieceBB & BLACK_QUEENSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (context.m_enemyAttackSet & BLACK_QUEENSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isQueensideClear && isNotThroughCheck) {
			params.m_moves.push_back(Move{QUIET_MOVE_BASE_SCORE, context.m_friendlyKingSquare, Square::c8, Piece::EMPTY, false, false, false, true});
		}
	}
}

Bitboard MoveGenerator::GetAttackSet(Bitboard pawnBB, Bitboard knightBB, Bitboard bishopBB, Bitboard rookBB, Bitboard queenBB, Bitboard kingBB, Bitboard emptySquareBB, Bitboard allPieceBB) const {
	Bitboard attackSet = 0ULL;

	if (m_board.IsWhiteTurn())
		attackSet |= GetBlackPawnAttackSet(pawnBB);
	else
		attackSet |= GetWhitePawnAttackSet(pawnBB);

	attackSet |= GetKnightAttackSet(knightBB);
	attackSet |= GetBishopAttackSet(bishopBB, allPieceBB);
	attackSet |= GetRookAttackSet(rookBB, allPieceBB);
	attackSet |= GetQueenAttackSet(queenBB, allPieceBB);
	attackSet |= GetKingAttackSet(kingBB);

	return attackSet;
}

Bitboard MoveGenerator::GetWhitePawnAttackSet(Bitboard pawns) const {
	Bitboard attackSet = 0ULL;

	attackSet |= pawns.ShiftNorthWest();
	attackSet |= pawns.ShiftNorthEast();

	return attackSet;
}

Bitboard MoveGenerator::GetBlackPawnAttackSet(Bitboard pawns) const {
	Bitboard attackSet = 0ULL;

	if (pawns.Empty()) return attackSet;

	attackSet |= pawns.ShiftSouthWest();
	attackSet |= pawns.ShiftSouthEast();

	return attackSet;
}

Bitboard MoveGenerator::GetKnightAttackSet(Bitboard knights) const {
	Bitboard attackSet = 0ULL;

	if (knights.Empty()) return attackSet;

	for (Square knight : knights) {
		attackSet |= m_magicBitboardHelper.GetKnightAttacks(knight);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetBishopAttackSet(Bitboard bishops, Bitboard allPieces) const {
	Bitboard attackSet{0ULL};

	for (Square bishop : bishops) {
		Bitboard occupancyMask = GetDiagonalOccupancyMask(bishop);

		Bitboard occupancy = occupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetDiagonalAttacks(bishop, occupancy);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetRookAttackSet(Bitboard rooks, Bitboard allPieces) const {
	Bitboard attackSet{0ULL};

	for (Square rook : rooks) {
		Bitboard occupancyMask = GetOrthogonalOccupancyMask(rook);
		Bitboard occupancy = occupancyMask & allPieces;

		Bitboard attacks = m_magicBitboardHelper.GetOrthogonalAttacks(rook, occupancy);

		attackSet |= attacks;
	}

	return attackSet;
}

Bitboard MoveGenerator::GetQueenAttackSet(Bitboard queens, Bitboard allPieces) const {
	Bitboard attackSet = 0ULL;
	
	if (queens.Empty()) return attackSet;

	for (Square queen : queens) {
		Bitboard orthogonalOccupancyMask = GetOrthogonalOccupancyMask(queen);
		Bitboard orthogonalOccupancy = orthogonalOccupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetOrthogonalAttacks(queen, orthogonalOccupancy);

		Bitboard diagonalOccupancyMask = GetDiagonalOccupancyMask(queen);
		Bitboard diagonalOccupancy = diagonalOccupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetDiagonalAttacks(queen, diagonalOccupancy);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetKingAttackSet(Bitboard king) const {
	Bitboard attackSet = m_magicBitboardHelper.GetKingAttacks(static_cast<Square>(king));

	return attackSet;
}
