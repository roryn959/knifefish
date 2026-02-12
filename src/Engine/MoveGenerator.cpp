#include "Engine/MoveGenerator.h"

// Skipping for now: en passant, castling, promotion

MoveGenerator::MoveGenerator(const Board& board) :
	m_board{board}
{}

std::vector<Move> MoveGenerator::GenerateMoves() {
	std::vector<Move> moves;
	if (m_board.IsWhiteTurn())
		GenerateWhiteMoves(moves);
	else
		GenerateBlackMoves(moves);
	
	return moves;
}

void MoveGenerator::GenerateWhiteMoves(std::vector<Move>& moves) {
	m_friendlyPieces = m_board.GetWhitePieceBitboard();
	m_enemyPieces = m_board.GetBlackPieceBitboard();

	m_viableSquares = ~m_friendlyPieces;
	m_emptySquares = ~m_board.GetAllPieceBitboard();

	m_pawns = m_board.GetPieceBitboard(Piece::WHITE_PAWN);
	m_knights = m_board.GetPieceBitboard(Piece::WHITE_KNIGHT);
	m_bishops = m_board.GetPieceBitboard(Piece::WHITE_BISHOP);
	m_rooks = m_board.GetPieceBitboard(Piece::WHITE_ROOK);
	m_queens = m_board.GetPieceBitboard(Piece::WHITE_QUEEN);
	m_king = m_board.GetPieceBitboard(Piece::WHITE_KING);

	GenerateWhitePawnMoves(moves);
	GenerateKnightMoves(moves);
	GenerateBishopMoves(moves);
	GenerateRookMoves(moves);
	GenerateQueenMoves(moves);
	GenerateKingMoves(moves);
}

void MoveGenerator::GenerateBlackMoves(std::vector<Move>& moves) {
	m_friendlyPieces = m_board.GetBlackPieceBitboard();
	m_enemyPieces = m_board.GetWhitePieceBitboard();

	m_viableSquares = ~m_friendlyPieces;
	m_emptySquares = ~m_board.GetAllPieceBitboard();

	m_pawns = m_board.GetPieceBitboard(Piece::BLACK_PAWN);
	m_knights = m_board.GetPieceBitboard(Piece::BLACK_KNIGHT);
	m_bishops = m_board.GetPieceBitboard(Piece::BLACK_BISHOP);
	m_rooks = m_board.GetPieceBitboard(Piece::BLACK_ROOK);
	m_queens = m_board.GetPieceBitboard(Piece::BLACK_QUEEN);
	m_king = m_board.GetPieceBitboard(Piece::BLACK_KING);

	GenerateBlackPawnMoves(moves);
	GenerateKnightMoves(moves);
	GenerateBishopMoves(moves);
	GenerateRookMoves(moves);
	GenerateQueenMoves(moves);
	GenerateKingMoves(moves);
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

	moves.push_back(Move{pawn, northMove});

	Bitboard secondRank = pawn & RANK_2_MASK;

	if (secondRank.IsEmpty()) return;

	Bitboard northNorthMove = ShiftNorth(northMove) & m_emptySquares;
	if (!northNorthMove.IsEmpty()) {
		moves.push_back(Move{pawn, northNorthMove});
	}
}

void MoveGenerator::GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard westCaptureMove = ShiftWest(push) & m_enemyPieces;
	if (!westCaptureMove.IsEmpty())
		moves.push_back(Move{pawn, westCaptureMove});

	Bitboard eastCaptureMove = ShiftEast(push) & m_enemyPieces;
	if (!eastCaptureMove.IsEmpty())
		moves.push_back(Move{pawn, eastCaptureMove});
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

	moves.push_back(Move{pawn, southMove});

	Bitboard seventhRank = pawn & RANK_7_MASK;

	if (seventhRank.IsEmpty()) return;

	Bitboard southSouthMove = ShiftSouth(southMove) & m_emptySquares;
	if (!southSouthMove.IsEmpty()) {
		moves.push_back(Move{pawn, southSouthMove});
	}
}

void MoveGenerator::GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard westCaptureMove = ShiftWest(push) & m_enemyPieces;
	if (!westCaptureMove.IsEmpty())
		moves.push_back(Move{pawn, westCaptureMove});

	Bitboard eastCaptureMove = ShiftEast(push) & m_enemyPieces;
	if (!eastCaptureMove.IsEmpty())
		moves.push_back(Move{pawn, eastCaptureMove});
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

	Bitboard nnw = ShiftWest(north) & m_viableSquares;
	if (!nnw.IsEmpty())
		moves.push_back(Move{knight, nnw});

	Bitboard nne = ShiftEast(north) & m_viableSquares;
	if (!nne.IsEmpty())
		moves.push_back(Move{knight, nne});
}

void MoveGenerator::GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight) {
	Bitboard east = (knight & NON_FILE_G_OR_H_MASK) >> 2;

	if (east.IsEmpty()) return;

	Bitboard een = ShiftNorth(east) & m_viableSquares;
	if (!een.IsEmpty())
		moves.push_back(Move{knight, een});
	
	Bitboard ees = ShiftSouth(east) & m_viableSquares;
	if (!ees.IsEmpty())
		moves.push_back(Move{knight, ees});
}

void MoveGenerator::GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight) {
	Bitboard south = (knight >> 16);

	if (south.IsEmpty()) return;

	Bitboard ssw = ShiftWest(south) & m_viableSquares;
	if (!ssw.IsEmpty())
		moves.push_back(Move{knight, ssw});

	Bitboard sse = ShiftEast(south) & m_viableSquares;
	if (!sse.IsEmpty())
		moves.push_back(Move{knight, sse});
}

void MoveGenerator::GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight) {
	Bitboard west = (knight & NON_FILE_A_OR_B_MASK) << 2;

	if (west.IsEmpty()) return;

	Bitboard wwn = ShiftNorth(west) & m_viableSquares;
	if (!wwn.IsEmpty())
		moves.push_back(Move{knight, wwn});
	
	Bitboard wws = ShiftSouth(west) & m_viableSquares;
	if (!wws.IsEmpty())
		moves.push_back(Move{knight, wws});
}

void MoveGenerator::GeneratePieceSlideNorthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftNorthEast(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftNorthEast(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideSouthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftSouthEast(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftSouthEast(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideSouthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftSouthWest(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftSouthWest(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideNorthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftNorthWest(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftNorthWest(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideNorth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftNorth(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftNorth(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftEast(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftEast(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideSouth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftSouth(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftSouth(shadow);
	}
}

void MoveGenerator::GeneratePieceSlideWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = ShiftWest(piece);
	while (!shadow.IsEmpty()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (!capture.IsEmpty()) {
			moves.push_back(Move{piece, shadow});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (!blocker.IsEmpty()) {
			return;
		}

		moves.push_back(Move{piece, shadow});
		shadow = ShiftWest(shadow);
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
	Bitboard northMove = ShiftNorth(m_king) & m_viableSquares;
	if (!northMove.IsEmpty()) {
		moves.push_back(Move{m_king, northMove});
	}

	Bitboard east = ShiftEast(m_king);

	Bitboard eastMove = east & m_viableSquares;
	if (!eastMove.IsEmpty()) {
		moves.push_back(Move{m_king, eastMove});
	}

	if (!east.IsEmpty()) {
		Bitboard northEastMove = ShiftNorth(east) & m_viableSquares;
		if (!northEastMove.IsEmpty()) {
			moves.push_back(Move{m_king, northEastMove});
		}

		Bitboard southEastMove = ShiftSouth(east) & m_viableSquares;
		if (!southEastMove.IsEmpty()) {
			moves.push_back(Move{m_king, southEastMove});
		}
	}

	Bitboard southMove = ShiftSouth(m_king) & m_viableSquares;
	if (!southMove.IsEmpty()) {
		moves.push_back(Move{m_king, southMove});
	}

	Bitboard west = ShiftWest(m_king);

	Bitboard westMove = west & m_viableSquares;
	if (!westMove.IsEmpty()) {
		moves.push_back(Move{m_king, westMove});
	}

	if (!west.IsEmpty()) {
		Bitboard northWestMove = ShiftNorth(west) & m_viableSquares;
		if (!northWestMove.IsEmpty()) {
			moves.push_back(Move{m_king, northWestMove});
		}

		Bitboard southWestMove = ShiftSouth(west) & m_viableSquares;
		if (!southWestMove.IsEmpty()) {
			moves.push_back(Move{m_king, southWestMove});
		}
	}
}