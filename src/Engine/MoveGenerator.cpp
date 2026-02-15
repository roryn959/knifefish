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

		if ((m_board.GetEnPassantSquare() & bitPosition))//((m_attackSet & bitPosition)) 
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
	if (!m_pawns) return;

	if (m_board.IsWhiteTurn()) {
		m_attackSet |= m_pawns.ShiftSouthWest();
		m_attackSet |= m_pawns.ShiftSouthEast();
	} else {
		m_attackSet |= m_pawns.ShiftNorthWest();
		m_attackSet |= m_pawns.ShiftNorthEast();
	}
}

void MoveGenerator::GenerateKnightAttackSet() {
	if (!m_knights) return;

	Bitboard north = m_knights.ShiftNorth();
	m_attackSet |= north.ShiftNorthWest();
	m_attackSet |= north.ShiftNorthEast();

	Bitboard east = m_knights.ShiftEast();
	m_attackSet |= east.ShiftNorthEast();
	m_attackSet |= east.ShiftSouthEast();

	Bitboard south = m_knights.ShiftSouth();
	m_attackSet |= south.ShiftSouthEast();
	m_attackSet |= south.ShiftSouthWest();

	Bitboard west = m_knights.ShiftWest();
	m_attackSet |= west.ShiftNorthWest();
	m_attackSet |= west.ShiftSouthWest();
}

void MoveGenerator::GenerateBishopAttackSet() {
	if (!m_bishops) return;

	Bitboard shadows;

	shadows = m_bishops;
	shadows = shadows.ShiftNorthEast();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftNorthEast();
	}

	shadows = m_bishops;
	shadows = shadows.ShiftSouthEast();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftSouthEast();
	}

	shadows = m_bishops;
	shadows = shadows.ShiftSouthWest();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftSouthWest();
	}

	shadows = m_bishops;
	shadows = shadows.ShiftNorthWest();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftNorthWest();
	}
}

void MoveGenerator::GenerateRookAttackSet() {
	if (!m_rooks) return;

	Bitboard shadows;
	
	shadows = m_rooks;
	shadows = shadows.ShiftNorth();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftNorth();
	}

	shadows = m_rooks;
	shadows = shadows.ShiftEast();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftEast();
	}

	shadows = m_rooks;
	shadows = shadows.ShiftSouth();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftSouth();
	}

	shadows = m_rooks;
	shadows = shadows.ShiftWest();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftWest();
	}
}

void MoveGenerator::GenerateQueenAttackSet() {
	if (!m_queens) return;

	Bitboard shadows;

	shadows = m_queens;
	shadows = shadows.ShiftNorthEast();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftNorthEast();
	}

	shadows = m_queens;
	shadows = shadows.ShiftSouthEast();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftSouthEast();
	}

	shadows = m_queens;
	shadows = shadows.ShiftSouthWest();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftSouthWest();
	}

	shadows = m_queens;
	shadows = shadows.ShiftNorthWest();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftNorthWest();
	}

	shadows = m_queens;
	shadows = shadows.ShiftNorth();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftNorth();
	}

	shadows = m_queens;
	shadows = shadows.ShiftEast();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftEast();
	}

	shadows = m_queens;
	shadows = shadows.ShiftSouth();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftSouth();
	}

	shadows = m_queens;
	shadows = shadows.ShiftWest();
	while (shadows) {
		m_attackSet |= shadows;
		shadows &= m_emptySquares;
		shadows = shadows.ShiftWest();
	}
}

void MoveGenerator::GenerateKingAttackSet() {
	m_attackSet |= m_king.ShiftNorth();
	m_attackSet |= m_king.ShiftNorthEast();
	m_attackSet |= m_king.ShiftEast();
	m_attackSet |= m_king.ShiftSouthEast();
	m_attackSet |= m_king.ShiftSouth();
	m_attackSet |= m_king.ShiftSouthWest();
	m_attackSet |= m_king.ShiftWest();
	m_attackSet |= m_king.ShiftNorthWest();
}

void MoveGenerator::GenerateWhitePawnMoves(std::vector<Move>& moves) {
	while (m_pawns) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = pawn.ShiftNorth();
		
		GenerateWhitePawnPushes(moves, pawn, push);
		GenerateWhitePawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard northMove = push & m_emptySquares;
	
	if (!northMove) return;

	Bitboard seventhRank = pawn & RANK_7_MASK;
	if (seventhRank) {
		moves.push_back(Move{pawn, northMove, Piece::WHITE_KNIGHT, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_BISHOP, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_ROOK, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{pawn, northMove, Piece::EMPTY, false, false, false, false});
	}

	Bitboard secondRank = pawn & RANK_2_MASK;
	if (!secondRank) return;

	Bitboard northNorthMove = northMove.ShiftNorth() & m_emptySquares;
	if (northNorthMove) {
		moves.push_back(Move{pawn, northNorthMove, Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard seventhRank = pawn & RANK_7_MASK;

	Bitboard westCaptureMove = push.ShiftWest() & m_enemyPieces;
	if (westCaptureMove) {
		if (seventhRank) {
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, westCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = push.ShiftEast() & m_enemyPieces;
	if (eastCaptureMove) {
		if (seventhRank) {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard westEnPassantMove = push.ShiftWest() & m_board.GetEnPassantSquare();
	if (westEnPassantMove) {
		moves.push_back(Move{pawn, westEnPassantMove, Piece::EMPTY, false, false, true, false});
	}

	Bitboard eastEnPassantMove = push.ShiftEast() & m_board.GetEnPassantSquare();
	if (eastEnPassantMove) {
		moves.push_back(Move{pawn, eastEnPassantMove, Piece::EMPTY, false, false, true, false});
	}
}

void MoveGenerator::GenerateBlackPawnMoves(std::vector<Move>& moves) {
	while (m_pawns) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = pawn.ShiftSouth();
		
		GenerateBlackPawnPushes(moves, pawn, push);
		GenerateBlackPawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateBlackPawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard southMove = push & m_emptySquares;
	
	if (!southMove) return;

	Bitboard secondRank = pawn & RANK_2_MASK;
	if (secondRank) {
		moves.push_back(Move{pawn, southMove, Piece::BLACK_KNIGHT, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_BISHOP, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_ROOK, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{pawn, southMove, Piece::EMPTY, false, false, false, false});
	}

	Bitboard seventhRank = pawn & RANK_7_MASK;
	if (!seventhRank) return;

	Bitboard southSouthMove = southMove.ShiftSouth() & m_emptySquares;
	if (southSouthMove) {
		moves.push_back(Move{pawn, southSouthMove, Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard secondRank = pawn & RANK_2_MASK;

	Bitboard westCaptureMove = push.ShiftWest() & m_enemyPieces;
	if (westCaptureMove) {
		if (secondRank) {
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, westCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = push.ShiftEast() & m_enemyPieces;
	if (eastCaptureMove) {
		if (secondRank) {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard westEnPassantMove = push.ShiftWest() & m_board.GetEnPassantSquare();
	if (westEnPassantMove) {
		moves.push_back(Move{pawn, westEnPassantMove, Piece::EMPTY, false, false, true, false});
	}

	Bitboard eastEnPassantMove = push.ShiftEast() & m_board.GetEnPassantSquare();
	if (eastEnPassantMove) {
		moves.push_back(Move{pawn, eastEnPassantMove, Piece::EMPTY, false, false, true, false});
	}
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) {
	while (m_knights) {
		Bitboard knight = m_knights.PopLsb();

		GenerateKnightHopsNorth(moves, knight);
		GenerateKnightHopsEast(moves, knight);
		GenerateKnightHopsSouth(moves, knight);
		GenerateKnightHopsWest(moves, knight);
	}
}

void MoveGenerator::GenerateKnightHopsNorth(std::vector<Move>& moves, Bitboard knight) {
	Bitboard north = (knight << 16);

	if (!north) return;
	
	Bitboard nnw = north.ShiftWest();
	if ((nnw & m_enemyPieces)) {
		moves.push_back(Move{knight, nnw, Piece::EMPTY, true, false, false, false});
	} else if ((nnw & m_emptySquares)) {
		moves.push_back(Move{knight, nnw, Piece::EMPTY, false, false, false, false});
	}

	Bitboard nne = north.ShiftEast();
	if ((nne & m_enemyPieces)) {
		moves.push_back(Move{knight, nne, Piece::EMPTY, true, false, false, false});
	} else if ((nne & m_emptySquares)) {
		moves.push_back(Move{knight, nne, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight) {
	Bitboard east = (knight & NON_FILE_G_OR_H_MASK) >> 2;

	if (!east) return;

	Bitboard een = east.ShiftNorth();
	if ((een & m_enemyPieces)) {
		moves.push_back(Move{knight, een, Piece::EMPTY, true, false, false, false});
	} else if ((een & m_emptySquares)) {
		moves.push_back(Move{knight, een, Piece::EMPTY, false, false, false, false});
	}
	
	Bitboard ees = east.ShiftSouth();
	if ((ees & m_enemyPieces)) {
		moves.push_back(Move{knight, ees, Piece::EMPTY, true, false, false, false});
	} else if ((ees & m_emptySquares)) {
		moves.push_back(Move{knight, ees, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight) {
	Bitboard south = (knight >> 16);

	if (!south) return;

	Bitboard ssw = south.ShiftWest();
	if ((ssw & m_enemyPieces)) {
		moves.push_back(Move{knight, ssw, Piece::EMPTY, true, false, false, false});
	} else if ((ssw & m_emptySquares)) {
		moves.push_back(Move{knight, ssw, Piece::EMPTY, false, false, false, false});
	}

	Bitboard sse = south.ShiftEast();
	if ((sse & m_enemyPieces)) {
		moves.push_back(Move{knight, sse, Piece::EMPTY, true, false, false, false});
	} else if ((sse & m_emptySquares)) {
		moves.push_back(Move{knight, sse, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight) {
	Bitboard west = (knight & NON_FILE_A_OR_B_MASK) << 2;

	if (!west) return;

	Bitboard wwn = west.ShiftNorth();
	if ((wwn & m_enemyPieces)) {
		moves.push_back(Move{knight, wwn, Piece::EMPTY, true, false, false, false});
	} else if ((wwn & m_emptySquares)) {
		moves.push_back(Move{knight, wwn, Piece::EMPTY, false, false, false, false});
	}
	
	Bitboard wws = west.ShiftSouth();
	if ((wws & m_enemyPieces)) {
		moves.push_back(Move{knight, wws, Piece::EMPTY, true, false, false, false});
	} else if ((wws & m_emptySquares)) {
		moves.push_back(Move{knight, wws, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GeneratePieceSlideNorthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftNorthEast();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftNorthEast();
	}
}

void MoveGenerator::GeneratePieceSlideSouthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftSouthEast();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftSouthEast();
	}
}

void MoveGenerator::GeneratePieceSlideSouthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftSouthWest();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftSouthWest();
	}
}

void MoveGenerator::GeneratePieceSlideNorthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftNorthWest();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftNorthWest();
	}
}

void MoveGenerator::GeneratePieceSlideNorth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftNorth();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftNorth();
	}
}

void MoveGenerator::GeneratePieceSlideEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftEast();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftEast();
	}
}

void MoveGenerator::GeneratePieceSlideSouth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftSouth();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftSouth();
	}
}

void MoveGenerator::GeneratePieceSlideWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftWest();
	while (shadow) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftWest();
	}
}

void MoveGenerator::GenerateKingNorthMove(std::vector<Move>& moves) {
	Bitboard north = m_king.ShiftNorth();

	if (!north) return;

	if ((north & m_enemyPieces)) {
		moves.push_back(Move{m_king, north, Piece::EMPTY, true, false, false, false});
	} else if ((north & m_emptySquares)) {
		moves.push_back(Move{m_king, north, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingEastMoves(std::vector<Move>& moves) {
	Bitboard east = m_king.ShiftEast();

	if (!east) return;

	if ((east & m_enemyPieces)) {
		moves.push_back(Move{m_king, east, Piece::EMPTY, true, false, false, false});
	} else if ((east & m_emptySquares)) {
		moves.push_back(Move{m_king, east, Piece::EMPTY, false, false, false, false});
	}

	Bitboard northEast = east.ShiftNorth();
	if ((northEast & m_enemyPieces)) {
		moves.push_back(Move{m_king, northEast, Piece::EMPTY, true, false, false, false});
	} else if ((northEast & m_emptySquares)) {
		moves.push_back(Move{m_king, northEast, Piece::EMPTY, false, false, false, false});
	}

	Bitboard southEast = east.ShiftSouth();
	if ((southEast & m_enemyPieces)) {
		moves.push_back(Move{m_king, southEast, Piece::EMPTY, true, false, false, false});
	} else if ((southEast & m_emptySquares)) {
		moves.push_back(Move{m_king, southEast, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingSouthMove(std::vector<Move>& moves) {
	Bitboard south = m_king.ShiftSouth();

	if (!south) return;

	if ((south & m_enemyPieces)) {
		moves.push_back(Move{m_king, south, Piece::EMPTY, true, false, false, false});
	} else if ((south & m_emptySquares)) {
		moves.push_back(Move{m_king, south, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingWestMoves(std::vector<Move>& moves) {
	Bitboard west = m_king.ShiftWest();

	if (!west) return;

	if ((west & m_enemyPieces)) {
		moves.push_back(Move{m_king, west, Piece::EMPTY, true, false, false, false});
	} else if ((west & m_emptySquares)) {
		moves.push_back(Move{m_king, west, Piece::EMPTY, false, false, false, false});
	}

	Bitboard northWest = west.ShiftNorth();
	if ((northWest & m_enemyPieces)) {
		moves.push_back(Move{m_king, northWest, Piece::EMPTY, true, false, false, false});
	} else if ((northWest & m_emptySquares)) {
		moves.push_back(Move{m_king, northWest, Piece::EMPTY, false, false, false, false});
	}

	Bitboard southWest = west.ShiftSouth();
	if ((southWest & m_enemyPieces)) {
		moves.push_back(Move{m_king, southWest, Piece::EMPTY, true, false, false, false});
	} else if ((southWest & m_emptySquares)) {
		moves.push_back(Move{m_king, southWest, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) {
	while (m_bishops) {
		Bitboard bishop = m_bishops.PopLsb();

		GeneratePieceSlideNorthEast(moves, bishop);
		GeneratePieceSlideSouthEast(moves, bishop);
		GeneratePieceSlideSouthWest(moves, bishop);
		GeneratePieceSlideNorthWest(moves, bishop);
	}
}

void MoveGenerator::GenerateRookMoves(std::vector<Move>& moves) {
	while (m_rooks) {
		Bitboard rook = m_rooks.PopLsb();

		GeneratePieceSlideNorth(moves, rook);
		GeneratePieceSlideEast(moves, rook);
		GeneratePieceSlideSouth(moves, rook);
		GeneratePieceSlideWest(moves, rook);
	}
}

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) {
	while (m_queens) {
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
		bool isKingsideClear = !(m_occupiedSquares & WHITE_KINGSIDE_CASTLE_SPACE_MASK);
		if (isKingsideClear) {
			moves.push_back(Move{m_king, G1_MASK, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.IsWhiteQueensideCastlePermitted()) {
		bool isQueensideClear = !(m_occupiedSquares & WHITE_QUEENSIDE_CASTLE_SPACE_MASK);
		if (isQueensideClear) {
			moves.push_back(Move{m_king, C1_MASK, Piece::EMPTY, false, false, false, true});
		}
	}
}

void MoveGenerator::GenerateBlackCastleMoves(std::vector<Move>& moves) {
	if (m_board.IsBlackKingsideCastlePermitted()) {
		bool isKingsideClear = !(m_occupiedSquares & BLACK_KINGSIDE_CASTLE_SPACE_MASK);
		if (isKingsideClear) {
			moves.push_back(Move{m_king, G8_MASK, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.IsBlackQueensideCastlePermitted()) {
		bool isQueensideClear = !(m_occupiedSquares & BLACK_QUEENSIDE_CASTLE_SPACE_MASK);
		if (isQueensideClear) {
			moves.push_back(Move{m_king, C8_MASK, Piece::EMPTY, false, false, false, true});
		}
	}
}