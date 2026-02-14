#include "Engine/MoveGenerator.h"

// Currently skipped: en passant, castling, promotion
// Potential performance increase by making capture checking branchless

MoveGenerator::MoveGenerator(const Board& board) :
	m_board{board}
{}

std::vector<Move> MoveGenerator::GenerateMoves() {
	std::vector<Move> pseudoMoves = GeneratePseudoMoves();

	return pseudoMoves;
}

std::vector<Move> MoveGenerator::GeneratePseudoMoves() {
	m_occupiedSquares = m_board.GetAllPieceBitboard();
	m_emptySquares = ~m_occupiedSquares;

	std::vector<Move> moves;
	if (m_board.IsWhiteTurn()) {
		GenerateAttackSet();
		PrepareWhiteMoveGeneration();
		GenerateWhitePawnMoves(moves);
		GenerateWhiteCastleMoves(moves);
	}
	else {
		GenerateAttackSet();
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

	m_emptySquares = ~m_board.GetAllPieceBitboard();

	m_pawns = m_board.GetPieceBitboard(Piece::BLACK_PAWN);
	m_knights = m_board.GetPieceBitboard(Piece::BLACK_KNIGHT);
	m_bishops = m_board.GetPieceBitboard(Piece::BLACK_BISHOP);
	m_rooks = m_board.GetPieceBitboard(Piece::BLACK_ROOK);
	m_queens = m_board.GetPieceBitboard(Piece::BLACK_QUEEN);
	m_king = m_board.GetPieceBitboard(Piece::BLACK_KING);
}

void MoveGenerator::GenerateAttackSet() {

	if (m_board.IsWhiteTurn())
		PrepareBlackMoveGeneration();
	else
		PrepareWhiteMoveGeneration();

	m_attackSet = 0ULL;

	GeneratePawnAttackSet();
	GenerateKnightAttackSet();
	GenerateBishopAttackSet();
	GenerateRookAttackSet();
	GenerateQueenAttackSet();
	GenerateKingAttackSet();

	// ----------

	std::cout << '\n';

	char boardArray[64];
	for (int i = 0; i < 64; ++i) {
		Bitboard bitPosition = 1ULL << i;

		if (!(m_attackSet & bitPosition).IsEmpty()) 
			boardArray[i] = 'X';
		else
			boardArray[i] = 'O';
	}

	int col = 0;
	for (int i = 63; i >= 0; --i) {

		std::cout << boardArray[i] << ' ';

		if (++col == 8) {
			std::cout << '\n';
			col = 0;
		}
	}

	std::cout << '\n';

	// ----------
}

void MoveGenerator::GeneratePawnAttackSet() {
	if (m_pawns.IsEmpty()) return;

	if (m_board.IsWhiteTurn()) {
		m_attackSet |= ShiftSouthWest(m_pawns);
		m_attackSet |= ShiftSouthEast(m_pawns);
	} else {
		m_attackSet |= ShiftNorthWest(m_pawns);
		m_attackSet |= ShiftNorthEast(m_pawns);
	}
}

void MoveGenerator::GenerateKnightAttackSet() {
	if (m_knights.IsEmpty()) return;

	Bitboard north = ShiftNorth(m_knights);
	m_attackSet |= ShiftNorthWest(north);
	m_attackSet |= ShiftNorthEast(north);

	Bitboard east = ShiftEast(m_knights);
	m_attackSet |= ShiftNorthEast(east);
	m_attackSet |= ShiftSouthEast(east);

	Bitboard south = ShiftSouth(m_knights);
	m_attackSet |= ShiftSouthEast(south);
	m_attackSet |= ShiftSouthWest(south);

	Bitboard west = ShiftWest(m_knights);
	m_attackSet |= ShiftNorthWest(west);
	m_attackSet |= ShiftSouthWest(west);
}

void MoveGenerator::GenerateBishopAttackSet() {
	if (m_bishops.IsEmpty()) return;

	Bitboard shadows;

	shadows = m_bishops;
	shadows = ShiftNorthEast(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftNorthEast(shadows);
	}

	shadows = m_bishops;
	shadows = ShiftSouthEast(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftSouthEast(shadows);
	}

	shadows = m_bishops;
	shadows = ShiftSouthWest(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftSouthWest(shadows);
	}

	shadows = m_bishops;
	shadows = ShiftNorthWest(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftNorthWest(shadows);
	}
}

void MoveGenerator::GenerateRookAttackSet() {
	if (m_rooks.IsEmpty()) return;

	Bitboard shadows;
	
	shadows = m_rooks;
	shadows = ShiftNorth(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftNorth(shadows);
	}

	shadows = m_rooks;
	shadows = ShiftEast(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftEast(shadows);
	}

	shadows = m_rooks;
	shadows = ShiftSouth(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftSouth(shadows);
	}

	shadows = m_rooks;
	shadows = ShiftWest(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftWest(shadows);
	}
}

void MoveGenerator::GenerateQueenAttackSet() {
	if (m_queens.IsEmpty()) return;

	Bitboard shadows;

	shadows = m_queens;
	shadows = ShiftNorthEast(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftNorthEast(shadows);
	}

	shadows = m_queens;
	shadows = ShiftSouthEast(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftSouthEast(shadows);
	}

	shadows = m_queens;
	shadows = ShiftSouthWest(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftSouthWest(shadows);
	}

	shadows = m_queens;
	shadows = ShiftNorthWest(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftNorthWest(shadows);
	}

	shadows = m_queens;
	shadows = ShiftNorth(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftNorth(shadows);
	}

	shadows = m_queens;
	shadows = ShiftEast(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftEast(shadows);
	}

	shadows = m_queens;
	shadows = ShiftSouth(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftSouth(shadows);
	}

	shadows = m_queens;
	shadows = ShiftWest(shadows);
	while (!shadows.IsEmpty()) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = ShiftWest(shadows);
	}
}

void MoveGenerator::GenerateKingAttackSet() {
	m_attackSet |= ShiftNorth(m_king);
	m_attackSet |= ShiftNorthEast(m_king);
	m_attackSet |= ShiftEast(m_king);
	m_attackSet |= ShiftSouthEast(m_king);
	m_attackSet |= ShiftSouth(m_king);
	m_attackSet |= ShiftSouthWest(m_king);
	m_attackSet |= ShiftWest(m_king);
	m_attackSet |= ShiftNorthWest(m_king);
}

void MoveGenerator::GenerateWhitePawnMoves(std::vector<Move>& moves) {
	while (!m_pawns.IsEmpty()) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = ShiftNorth(pawn);
		
		GenerateWhitePawnPushes(moves, pawn, push);
		GenerateWhitePawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard northMove = push & m_emptySquares;
	
	if (northMove.IsEmpty()) return;

	bool seventhRank = !(pawn & RANK_7_MASK).IsEmpty();
	if (seventhRank) {
		moves.push_back(Move{pawn, northMove, Piece::WHITE_KNIGHT, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_BISHOP, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_ROOK, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{pawn, northMove, Piece::EMPTY, false, false, false, false});
	}

	bool secondRank = !(pawn & RANK_2_MASK).IsEmpty();
	if (!secondRank) return;

	Bitboard northNorthMove = ShiftNorth(northMove) & m_emptySquares;
	if (!northNorthMove.IsEmpty()) {
		moves.push_back(Move{pawn, northNorthMove, Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	bool seventhRank = !(pawn & RANK_7_MASK).IsEmpty();

	Bitboard westCaptureMove = ShiftWest(push) & m_enemyPieces;
	if (!westCaptureMove.IsEmpty()) {
		if (seventhRank) {
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, westCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = ShiftEast(push) & m_enemyPieces;
	if (!eastCaptureMove.IsEmpty()) {
		if (seventhRank) {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}
}

void MoveGenerator::GenerateBlackPawnMoves(std::vector<Move>& moves) {
	while (!m_pawns.IsEmpty()) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = ShiftSouth(pawn);
		
		GenerateBlackPawnPushes(moves, pawn, push);
		GenerateBlackPawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateBlackPawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard southMove = push & m_emptySquares;
	
	if (southMove.IsEmpty()) return;

	bool secondRank = !(pawn & RANK_2_MASK).IsEmpty();
	if (secondRank) {
		moves.push_back(Move{pawn, southMove, Piece::BLACK_KNIGHT, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_BISHOP, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_ROOK, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{pawn, southMove, Piece::EMPTY, false, false, false, false});
	}

	bool seventhRank = !(pawn & RANK_7_MASK).IsEmpty();
	if (!seventhRank) return;

	Bitboard southSouthMove = ShiftSouth(southMove) & m_emptySquares;
	if (!southSouthMove.IsEmpty()) {
		moves.push_back(Move{pawn, southSouthMove, Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	bool secondRank = !(pawn & RANK_2_MASK).IsEmpty();

	Bitboard westCaptureMove = ShiftWest(push) & m_enemyPieces;
	if (!westCaptureMove.IsEmpty()) {
		if (secondRank) {
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, westCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = ShiftEast(push) & m_enemyPieces;
	if (!eastCaptureMove.IsEmpty()) {
		if (secondRank) {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) {
	while (!m_knights.IsEmpty()) {
		Bitboard knight = m_knights.PopLsb();

		GenerateKnightHopsNorth(moves, knight);
		GenerateKnightHopsEast(moves, knight);
		GenerateKnightHopsSouth(moves, knight);
		GenerateKnightHopsWest(moves, knight);
	}
}

void MoveGenerator::GenerateKnightHopsNorth(std::vector<Move>& moves, Bitboard knight) {
	Bitboard north = (knight << 16);

	if (north.IsEmpty()) return;
	
	Bitboard nnw = ShiftWest(north);
	if (!(nnw & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, nnw, Piece::EMPTY, true, false, false, false});
	} else if (!(nnw & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, nnw, Piece::EMPTY, false, false, false, false});
	}

	Bitboard nne = ShiftEast(north);
	if (!(nne & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, nne, Piece::EMPTY, true, false, false, false});
	} else if (!(nne & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, nne, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight) {
	Bitboard east = (knight & NON_FILE_G_OR_H_MASK) >> 2;

	if (east.IsEmpty()) return;

	Bitboard een = ShiftNorth(east);
	if (!(een & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, een, Piece::EMPTY, true, false, false, false});
	} else if (!(een & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, een, Piece::EMPTY, false, false, false, false});
	}
	
	Bitboard ees = ShiftSouth(east);
	if (!(ees & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, ees, Piece::EMPTY, true, false, false, false});
	} else if (!(ees & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, ees, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight) {
	Bitboard south = (knight >> 16);

	if (south.IsEmpty()) return;

	Bitboard ssw = ShiftWest(south);
	if (!(ssw & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, ssw, Piece::EMPTY, true, false, false, false});
	} else if (!(ssw & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, ssw, Piece::EMPTY, false, false, false, false});
	}

	Bitboard sse = ShiftEast(south);
	if (!(sse & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, sse, Piece::EMPTY, true, false, false, false});
	} else if (!(sse & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, sse, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight) {
	Bitboard west = (knight & NON_FILE_A_OR_B_MASK) << 2;

	if (west.IsEmpty()) return;

	Bitboard wwn = ShiftNorth(west);
	if (!(wwn & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, wwn, Piece::EMPTY, true, false, false, false});
	} else if (!(wwn & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, wwn, Piece::EMPTY, false, false, false, false});
	}
	
	Bitboard wws = ShiftSouth(west);
	if (!(wws & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{knight, wws, Piece::EMPTY, true, false, false, false});
	} else if (!(wws & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{knight, wws, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GeneratePieceSlideNorthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftNorthEast(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftNorthEast(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideSouthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftSouthEast(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftSouthEast(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideSouthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftSouthWest(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftSouthWest(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideNorthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftNorthWest(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftNorthWest(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideNorth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftNorth(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftNorth(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftEast(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftEast(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideSouth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftSouth(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftSouth(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftWest(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = ShiftWest(shadow);
	}
}

void MoveGenerator::GenerateKingNorthMove(std::vector<Move>& moves) {
	Bitboard north = ShiftNorth(m_king);

	if (north.IsEmpty()) return;

	if (!(north & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, north, Piece::EMPTY, true, false, false, false});
	} else if (!(north & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, north, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingEastMoves(std::vector<Move>& moves) {
	Bitboard east = ShiftEast(m_king);

	if (east.IsEmpty()) return;

	if (!(east & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, east, Piece::EMPTY, true, false, false, false});
	} else if (!(east & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, east, Piece::EMPTY, false, false, false, false});
	}

	Bitboard northEast = ShiftNorth(east);
	if (!(northEast & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, northEast, Piece::EMPTY, true, false, false, false});
	} else if (!(northEast & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, northEast, Piece::EMPTY, false, false, false, false});
	}

	Bitboard southEast = ShiftSouth(east);
	if (!(southEast & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, southEast, Piece::EMPTY, true, false, false, false});
	} else if (!(southEast & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, southEast, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingSouthMove(std::vector<Move>& moves) {
	Bitboard south = ShiftSouth(m_king);

	if (south.IsEmpty()) return;

	if (!(south & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, south, Piece::EMPTY, true, false, false, false});
	} else if (!(south & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, south, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingWestMoves(std::vector<Move>& moves) {
	Bitboard west = ShiftWest(m_king);

	if (west.IsEmpty()) return;

	if (!(west & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, west, Piece::EMPTY, true, false, false, false});
	} else if (!(west & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, west, Piece::EMPTY, false, false, false, false});
	}

	Bitboard northWest = ShiftNorth(west);
	if (!(northWest & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, northWest, Piece::EMPTY, true, false, false, false});
	} else if (!(northWest & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, northWest, Piece::EMPTY, false, false, false, false});
	}

	Bitboard southWest = ShiftSouth(west);
	if (!(southWest & m_enemyPieces).IsEmpty()) {
		moves.push_back(Move{m_king, southWest, Piece::EMPTY, true, false, false, false});
	} else if (!(southWest & m_emptySquares).IsEmpty()) {
		moves.push_back(Move{m_king, southWest, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) {
	while (!m_bishops.IsEmpty()) {
		Bitboard bishop = m_bishops.PopLsb();

		GeneratePieceSlideNorthEast(moves, bishop);
		GeneratePieceSlideSouthEast(moves, bishop);
		GeneratePieceSlideSouthWest(moves, bishop);
		GeneratePieceSlideNorthWest(moves, bishop);
	}
}

void MoveGenerator::GenerateRookMoves(std::vector<Move>& moves) {
	while (!m_rooks.IsEmpty()) {
		Bitboard rook = m_rooks.PopLsb();

		GeneratePieceSlideNorth(moves, rook);
		GeneratePieceSlideEast(moves, rook);
		GeneratePieceSlideSouth(moves, rook);
		GeneratePieceSlideWest(moves, rook);
	}
}

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) {
	while (!m_queens.IsEmpty()) {
		Bitboard queen = m_queens.PopLsb();

		GeneratePieceSlideNorth(moves, queen);
		GeneratePieceSlideEast(moves, queen);
		GeneratePieceSlideSouth(moves, queen);
		GeneratePieceSlideWest(moves, queen);
		GeneratePieceSlideNorthEast(moves, queen);
		GeneratePieceSlideSouthEast(moves, queen);
		GeneratePieceSlideSouthWest(moves, queen);
		GeneratePieceSlideNorthWest(moves, queen);
	}
}

void MoveGenerator::GenerateKingMoves(std::vector<Move>& moves) {
	GenerateKingNorthMove(moves);
	GenerateKingEastMoves(moves);
	GenerateKingSouthMove(moves);
	GenerateKingWestMoves(moves);
}

void MoveGenerator::GenerateWhiteCastleMoves(std::vector<Move>& moves) {
	if (m_board.IsWhiteKingsideCastlePermitted()) {
		//bool isKingsideClear = !(m_emptySquares & WHITE_KINGSIDE_CASTLE_SPACE_MASK).IsEmpty();
		bool isKingsideClear = (m_occupiedSquares & WHITE_KINGSIDE_CASTLE_SPACE_MASK).IsEmpty();
		if (isKingsideClear) {
			moves.push_back(Move{m_king, G1_MASK, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.IsWhiteQueensideCastlePermitted()) {
		//bool isQueensideClear = !(m_emptySquares & WHITE_QUEENSIDE_CASTLE_SPACE_MASK).IsEmpty();
		bool isQueensideClear = (m_occupiedSquares & WHITE_QUEENSIDE_CASTLE_SPACE_MASK).IsEmpty();
		if (isQueensideClear) {
			moves.push_back(Move{m_king, C1_MASK, Piece::EMPTY, false, false, false, true});
		}
	}
}

void MoveGenerator::GenerateBlackCastleMoves(std::vector<Move>& moves) {
	if (m_board.IsBlackKingsideCastlePermitted()) {
		//bool isKingsideClear = !(m_emptySquares & BLACK_KINGSIDE_CASTLE_SPACE_MASK).IsEmpty();
		bool isKingsideClear = (m_occupiedSquares & BLACK_KINGSIDE_CASTLE_SPACE_MASK).IsEmpty();
		if (isKingsideClear) {
			moves.push_back(Move{m_king, G8_MASK, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.IsBlackQueensideCastlePermitted()) {
		//bool isQueensideClear = !(m_emptySquares & BLACK_QUEENSIDE_CASTLE_SPACE_MASK).IsEmpty();
		bool isQueensideClear = (m_occupiedSquares & BLACK_QUEENSIDE_CASTLE_SPACE_MASK).IsEmpty();
		if (isQueensideClear) {
			moves.push_back(Move{m_king, C8_MASK, Piece::EMPTY, false, false, false, true});
		}
	}
}