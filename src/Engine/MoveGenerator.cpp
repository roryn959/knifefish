#include "Engine/MoveGenerator.h"


MoveGenerator::MoveGenerator(Board& board) :
	m_board{board},
	m_magicBitboardHelper{}
{}

std::vector<Move> MoveGenerator::GenerateLegalMoves() {

#if DEBUG
	Hash beforeHash = m_board.GetHash();
#endif





	std::vector<Move> moves;

	Bitboard king = m_board.IsWhiteTurn() ? m_board.GetPieceBitboard(Piece::WHITE_KING) : m_board.GetPieceBitboard(Piece::BLACK_KING);

	








#if DEBUG
	Hash afterHash = m_board.GetHash();
	if (beforeHash != afterHash) {
		std::cerr << "Error: Hashing issue detected during move generation\n";
		std::exit(1);
	}
#endif

	return moves;
}

std::vector<Move> MoveGenerator::OldGenerateLegalMoves() {
	std::vector<Move> pseudoMoves = GeneratePseudoMoves();

#if DEBUG
	Hash beforeHash = m_board.GetHash();
#endif

	std::vector<Move> legalMoves;
	legalMoves.reserve(218);

	if (m_board.IsWhiteTurn())
		legalMoves = FilterOutIllegalWhiteMoves(pseudoMoves);
	else
		legalMoves = FilterOutIllegalBlackMoves(pseudoMoves);

#if DEBUG
	Hash afterHash = m_board.GetHash();
	if (beforeHash != afterHash) {
		std::cerr << "Error: Hashing issue detected during move generation\n";
		std::exit(1);
	}
#endif

	return legalMoves;
}

std::vector<Move> MoveGenerator::FilterOutIllegalWhiteMoves(std::vector<Move>& moves) {
	std::vector<Move> legalMoves;

	for (Move& move : moves) {
		Undo undo = m_board.MakeMove(move);

		Bitboard king = m_board.GetPieceBitboard(Piece::WHITE_KING);
		if (!IsAttackedByBlack(king))
			legalMoves.push_back(move);
		
		m_board.UndoMove(move, undo);
	}

	return legalMoves;
}

std::vector<Move> MoveGenerator::FilterOutIllegalBlackMoves(std::vector<Move>& moves) {
	std::vector<Move> legalMoves;

	for (Move& move : moves) {
		Undo undo = m_board.MakeMove(move);

		Bitboard king = m_board.GetPieceBitboard(Piece::BLACK_KING);
		if (!IsAttackedByWhite(king))
			legalMoves.push_back(move);
		
		m_board.UndoMove(move, undo);
	}

	return legalMoves;
} 

std::vector<Move> MoveGenerator::GeneratePseudoMoves() {
	m_occupiedSquares = m_board.GetAllPieceBitboard();
	m_emptySquares = ~m_occupiedSquares;

	std::vector<Move> moves;
	moves.reserve(218);

	if (m_board.IsWhiteTurn()) {
		PrepareWhiteMoveGeneration();
		GenerateWhitePawnMoves(moves);
		GenerateWhiteCastleMoves(moves);
	}
	else {
		PrepareBlackMoveGeneration();
		GenerateBlackPawnMoves(moves);
		GenerateBlackCastleMoves(moves);
	}

	GenerateKnightMoves(moves);
	GenerateBishopMoves(moves);
	GenerateRookMoves(moves);
	GenerateQueenMoves(moves);
	GenerateKingMoves(moves);
	
	return moves;
}

void MoveGenerator::PrepareWhiteMoveGeneration() {
	m_friendlyPieces = m_board.GetWhitePieceBitboard();
	m_enemyPieces = m_board.GetBlackPieceBitboard();
	m_enemyAttackSet = GetBlackAttackSet();

	m_pawns = m_board.GetPieceBitboard(Piece::WHITE_PAWN);
	m_knights = m_board.GetPieceBitboard(Piece::WHITE_KNIGHT);
	m_bishops = m_board.GetPieceBitboard(Piece::WHITE_BISHOP);
	m_rooks = m_board.GetPieceBitboard(Piece::WHITE_ROOK);
	m_queens = m_board.GetPieceBitboard(Piece::WHITE_QUEEN);
	m_king = m_board.GetPieceBitboard(Piece::WHITE_KING);
}

void MoveGenerator::PrepareBlackMoveGeneration() {
	m_friendlyPieces = m_board.GetBlackPieceBitboard();
	m_enemyPieces = m_board.GetWhitePieceBitboard();
	m_enemyAttackSet = GetWhiteAttackSet();

	m_pawns = m_board.GetPieceBitboard(Piece::BLACK_PAWN);
	m_knights = m_board.GetPieceBitboard(Piece::BLACK_KNIGHT);
	m_bishops = m_board.GetPieceBitboard(Piece::BLACK_BISHOP);
	m_rooks = m_board.GetPieceBitboard(Piece::BLACK_ROOK);
	m_queens = m_board.GetPieceBitboard(Piece::BLACK_QUEEN);
	m_king = m_board.GetPieceBitboard(Piece::BLACK_KING);
}

void MoveGenerator::GenerateWhitePawnMoves(std::vector<Move>& moves) {
	while (m_pawns.Any()) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = pawn.ShiftNorth();
		
		GenerateWhitePawnPushes(moves, pawn, push);
		GenerateWhitePawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard northMove = push & m_emptySquares;
	
	if (northMove.Empty()) return;

	Bitboard seventhRank = pawn & RANK_7_MASK;
	if (seventhRank.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(northMove), Piece::WHITE_KNIGHT, false, false, false, false});
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(northMove), Piece::WHITE_BISHOP, false, false, false, false});
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(northMove), Piece::WHITE_ROOK, false, false, false, false});
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(northMove), Piece::WHITE_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(northMove), Piece::EMPTY, false, false, false, false});
	}

	Bitboard secondRank = pawn & RANK_2_MASK;
	if (secondRank.Empty()) return;

	Bitboard northNorthMove = northMove.ShiftNorth() & m_emptySquares;
	if (northNorthMove.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(northNorthMove), Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard seventhRank = pawn & RANK_7_MASK;

	Bitboard westCaptureMove = push.ShiftWest() & m_enemyPieces;
	if (westCaptureMove.Any()) {
		if (seventhRank.Any()) {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = push.ShiftEast() & m_enemyPieces;
	if (eastCaptureMove.Any()) {
		if (seventhRank.Any()) {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(eastCaptureMove), Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(eastCaptureMove), Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(eastCaptureMove), Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(eastCaptureMove), Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(eastCaptureMove), Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard enPassantBB{m_board.GetEnPassantSquare()};

	Bitboard westEnPassantMove = push.ShiftWest() & enPassantBB;
	if (westEnPassantMove.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westEnPassantMove), Piece::EMPTY, false, false, true, false});
	}

	Bitboard eastEnPassantMove = push.ShiftEast() & enPassantBB;
	if (eastEnPassantMove.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(eastEnPassantMove), Piece::EMPTY, false, false, true, false});
	}
}

void MoveGenerator::GenerateBlackPawnMoves(std::vector<Move>& moves) {
	while (m_pawns.Any()) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = pawn.ShiftSouth();
		
		GenerateBlackPawnPushes(moves, pawn, push);
		GenerateBlackPawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateBlackPawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard southMove = push & m_emptySquares;
	
	if (southMove.Empty()) return;

	Bitboard secondRank = pawn & RANK_2_MASK;
	if (secondRank.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(southMove), Piece::BLACK_KNIGHT, false, false, false, false});
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(southMove), Piece::BLACK_BISHOP, false, false, false, false});
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(southMove), Piece::BLACK_ROOK, false, false, false, false});
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(southMove), Piece::BLACK_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(southMove), Piece::EMPTY, false, false, false, false});
	}

	Bitboard seventhRank = pawn & RANK_7_MASK;
	if (seventhRank.Empty()) return;

	Bitboard southSouthMove = southMove.ShiftSouth() & m_emptySquares;
	if (southSouthMove.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(southSouthMove), Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard secondRank = pawn & RANK_2_MASK;

	Bitboard westCaptureMove = push.ShiftWest() & m_enemyPieces;
	if (westCaptureMove.Any()) {
		if (secondRank.Any()) {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westCaptureMove), Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = push.ShiftEast() & m_enemyPieces;
	if (eastCaptureMove.Any()) {
		if (secondRank.Any()) {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(static_cast<Square>(eastCaptureMove)), Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(static_cast<Square>(eastCaptureMove)), Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(static_cast<Square>(eastCaptureMove)), Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(static_cast<Square>(eastCaptureMove)), Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(static_cast<Square>(eastCaptureMove)), Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard enPassantBB{m_board.GetEnPassantSquare()};

	Bitboard westEnPassantMove = push.ShiftWest() & enPassantBB;
	if (westEnPassantMove.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(westEnPassantMove), Piece::EMPTY, false, false, true, false});
	}

	Bitboard eastEnPassantMove = push.ShiftEast() & enPassantBB;
	if (eastEnPassantMove.Any()) {
		moves.push_back(Move{static_cast<Square>(pawn), static_cast<Square>(eastEnPassantMove), Piece::EMPTY, false, false, true, false});
	}
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) {
	for (Square knight : m_knights) {
		if (knight == Square::NONE) continue;

		Bitboard attackSet = m_magicBitboardHelper.GetKnightAttacks(knight);

		Bitboard normalMoves = attackSet & m_emptySquares;
		for (Square to : normalMoves) {
			moves.push_back(Move{knight, to, Piece::EMPTY, false, false, false, false});
		}

		Bitboard captures = attackSet & m_enemyPieces;
		for (Square to : captures) {
			moves.push_back(Move{knight, to, Piece::EMPTY, true, false, false, false});
		}
	}
}

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) {
	for (Square bishop : m_bishops) {
		Bitboard occupancyMask = GetDiagonalOccupancyMask(bishop);
		Bitboard occupancy = occupancyMask & m_occupiedSquares;

		Bitboard attackSet = m_magicBitboardHelper.GetDiagonalAttacks(bishop, occupancy);

		Bitboard normalMoves = attackSet & m_emptySquares;
		for (Square to : normalMoves) {
			moves.push_back(Move{bishop, to, Piece::EMPTY, false, false, false, false});
		}

		Bitboard captures = attackSet & m_enemyPieces;
		for (Square to : captures) {
			moves.push_back(Move{bishop, to, Piece::EMPTY, true, false, false, false});
		}
	}
}

void MoveGenerator::GenerateRookMoves(std::vector<Move>& moves) {
	for (Square rook : m_rooks) {
		Bitboard occupancyMask = GetOrthogonalOccupancyMask(rook);
		Bitboard occupancy = occupancyMask & m_occupiedSquares;

		Bitboard attackSet = m_magicBitboardHelper.GetOrthogonalAttacks(rook, occupancy);

		Bitboard normalMoves = attackSet & m_emptySquares;
		for (Square to : normalMoves) {
			moves.push_back(Move{rook, to, Piece::EMPTY, false, false, false, false});
		}

		Bitboard captures = attackSet & m_enemyPieces;
		for (Square to : captures) {
			moves.push_back(Move{rook, to, Piece::EMPTY, true, false, false, false});
		}
	}
}

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) {
	for (Square queen : m_queens) {
		Bitboard diagonalOccupancyMask = GetDiagonalOccupancyMask(queen);
		Bitboard diagonalOccupancy = diagonalOccupancyMask & m_occupiedSquares;
		Bitboard diagonalAttackSet = m_magicBitboardHelper.GetDiagonalAttacks(queen, diagonalOccupancy);

		Bitboard orthogonalOccupancyMask = GetOrthogonalOccupancyMask(queen);
		Bitboard orthogonalOccupancy = orthogonalOccupancyMask & m_occupiedSquares;
		Bitboard orthogonalAttackSet = m_magicBitboardHelper.GetOrthogonalAttacks(queen, orthogonalOccupancy);

		Bitboard attackSet = diagonalAttackSet | orthogonalAttackSet;

		Bitboard normalMoves = attackSet & m_emptySquares;
		for (Square to : normalMoves) {
			moves.push_back(Move{queen, to, Piece::EMPTY, false, false, false, false});
		}

		Bitboard captures = attackSet & m_enemyPieces;
		for (Square to : captures) {
			moves.push_back(Move{queen, to, Piece::EMPTY, true, false, false, false});
		}
	}
}

void MoveGenerator::GenerateKingMoves(std::vector<Move>& moves) {
	Square kingSquare = static_cast<Square>(m_king);
	Bitboard attackSet = m_magicBitboardHelper.GetKingAttacks(kingSquare);

	Bitboard normalMoves = attackSet & m_emptySquares;
	for (Square to : normalMoves) {
		moves.push_back(Move{kingSquare, to, Piece::EMPTY, false, false, false, false});
	}

	Bitboard captures = attackSet & m_enemyPieces;
	for (Square to : captures) {
		moves.push_back(Move{kingSquare, to, Piece::EMPTY, true, false, false, false});
	}
}

void MoveGenerator::GenerateWhiteCastleMoves(std::vector<Move>& moves) {
	if (m_board.GetCastlePermission(CastlePermission::WHITE_KINGSIDE)) {
		bool isKingsideClear = (m_occupiedSquares & WHITE_KINGSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & WHITE_KINGSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isKingsideClear && isNotThroughCheck) {
			moves.push_back(Move{static_cast<Square>(m_king), Square::g1, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.GetCastlePermission(CastlePermission::WHITE_QUEENSIDE)) {
		bool isQueensideClear = (m_occupiedSquares & WHITE_QUEENSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & WHITE_QUEENSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isQueensideClear && isNotThroughCheck) {
			moves.push_back(Move{static_cast<Square>(m_king), Square::c1, Piece::EMPTY, false, false, false, true});
		}
	}
}

void MoveGenerator::GenerateBlackCastleMoves(std::vector<Move>& moves) {
	if (m_board.GetCastlePermission(CastlePermission::BLACK_KINGSIDE)) {
		bool isKingsideClear = (m_occupiedSquares & BLACK_KINGSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & BLACK_KINGSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isKingsideClear && isNotThroughCheck) {
			moves.push_back(Move{static_cast<Square>(m_king), Square::g8, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.GetCastlePermission(CastlePermission::BLACK_QUEENSIDE)) {
		bool isQueensideClear = (m_occupiedSquares & BLACK_QUEENSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & BLACK_QUEENSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isQueensideClear && isNotThroughCheck) {
			moves.push_back(Move{static_cast<Square>(m_king), Square::c8, Piece::EMPTY, false, false, false, true});
		}
	}
}

bool MoveGenerator::IsAttackedByWhite(Bitboard square) {
	Bitboard whiteAttackSet = GetWhiteAttackSet();
	return (whiteAttackSet & square).Any();
}

bool MoveGenerator::IsAttackedByBlack(Bitboard square) {
	Bitboard blackAttackSet = GetBlackAttackSet();
	return (blackAttackSet & square).Any();
}

Bitboard MoveGenerator::GetWhiteAttackSet() {
	Bitboard allPieces = m_board.GetAllPieceBitboard();
	Bitboard emptySquares = ~allPieces;

	Bitboard pawns = m_board.GetPieceBitboard(Piece::WHITE_PAWN);
	Bitboard knights = m_board.GetPieceBitboard(Piece::WHITE_KNIGHT);
	Bitboard bishops = m_board.GetPieceBitboard(Piece::WHITE_BISHOP);
	Bitboard rooks = m_board.GetPieceBitboard(Piece::WHITE_ROOK);
	Bitboard queens = m_board.GetPieceBitboard(Piece::WHITE_QUEEN);
	Bitboard king = m_board.GetPieceBitboard(Piece::WHITE_KING);

	Bitboard attackSet = 0ULL;

	attackSet |= GetWhitePawnAttackSet(pawns);
	attackSet |= GetKnightAttackSet(knights);
	attackSet |= GetBishopAttackSet(bishops, allPieces);
	attackSet |= GetRookAttackSet(rooks, allPieces);
	attackSet |= GetQueenAttackSet(queens, allPieces);
	attackSet |= GetKingAttackSet(king);

	return attackSet;
}

Bitboard MoveGenerator::GetBlackAttackSet() {
	Bitboard allPieces = m_board.GetAllPieceBitboard();
	Bitboard emptySquares = ~allPieces;

	Bitboard pawns = m_board.GetPieceBitboard(Piece::BLACK_PAWN);
	Bitboard knights = m_board.GetPieceBitboard(Piece::BLACK_KNIGHT);
	Bitboard bishops = m_board.GetPieceBitboard(Piece::BLACK_BISHOP);
	Bitboard rooks = m_board.GetPieceBitboard(Piece::BLACK_ROOK);
	Bitboard queens = m_board.GetPieceBitboard(Piece::BLACK_QUEEN);
	Bitboard king = m_board.GetPieceBitboard(Piece::BLACK_KING);

	Bitboard attackSet = 0ULL;

	attackSet |= GetBlackPawnAttackSet(pawns);
	attackSet |= GetKnightAttackSet(knights);
	attackSet |= GetBishopAttackSet(bishops, allPieces);
	attackSet |= GetRookAttackSet(rooks, allPieces);
	attackSet |= GetQueenAttackSet(queens, allPieces);
	attackSet |= GetKingAttackSet(king);

	return attackSet;
}

Bitboard MoveGenerator::GetWhitePawnAttackSet(Bitboard pawns) {
	Bitboard attackSet = 0ULL;

	attackSet |= pawns.ShiftNorthWest();
	attackSet |= pawns.ShiftNorthEast();

	return attackSet;
}

Bitboard MoveGenerator::GetBlackPawnAttackSet(Bitboard pawns) {
	Bitboard attackSet = 0ULL;

	if (pawns.Empty()) return attackSet;

	attackSet |= pawns.ShiftSouthWest();
	attackSet |= pawns.ShiftSouthEast();

	return attackSet;
}

Bitboard MoveGenerator::GetKnightAttackSet(Bitboard knights) {
	Bitboard attackSet = 0ULL;

	if (knights.Empty()) return attackSet;

	for (Square knight : knights) {
		attackSet |= m_magicBitboardHelper.GetKnightAttacks(knight);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetBishopAttackSet(Bitboard bishops, Bitboard allPieces) {
	Bitboard attackSet{0ULL};

	for (Square bishop : bishops) {
		Bitboard occupancyMask = GetDiagonalOccupancyMask(bishop);

		Bitboard occupancy = occupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetDiagonalAttacks(bishop, occupancy);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetRookAttackSet(Bitboard rooks, Bitboard allPieces) {
	Bitboard attackSet{0ULL};

	for (Square rook : rooks) {
		Bitboard occupancyMask = GetOrthogonalOccupancyMask(rook);
		Bitboard occupancy = occupancyMask & allPieces;

		Bitboard attacks = m_magicBitboardHelper.GetOrthogonalAttacks(rook, occupancy);

		attackSet |= attacks;
	}

	return attackSet;
}

Bitboard MoveGenerator::GetQueenAttackSet(Bitboard queens, Bitboard allPieces) {
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

Bitboard MoveGenerator::GetKingAttackSet(Bitboard king) {
	Bitboard attackSet = m_magicBitboardHelper.GetKingAttacks(static_cast<Square>(king));

	return attackSet;
}
