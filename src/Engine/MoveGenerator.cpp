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
	//GenerateWhitePawnMoves(moves);
	GenerateWhiteKnightMoves(moves);
}

void MoveGenerator::GenerateWhitePawnMoves(std::vector<Move>& moves) {
	Bitboard pawns = m_board.GetPieceBitboard(Piece::WHITE_PAWN);
	Bitboard emptySquares = ~m_board.GetAllPieceBitboard();
	Bitboard enemyPieces = m_board.GetBlackPieceBitboard();

	while (!pawns.IsEmpty()) {
		Bitboard pawn = pawns.PopLsb();
		Bitboard push = ShiftNorth(pawn);
		
		GenerateWhitePawnPushes(moves, pawn, push, emptySquares);
		GenerateWhitePawnCaptures(moves, pawn, push, enemyPieces);
	}
}

void MoveGenerator::GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push, Bitboard emptySquares) {
	Bitboard northMove = push & emptySquares;
	
	if (northMove.IsEmpty()) return;

	moves.push_back(Move{pawn, northMove});

	Bitboard secondRank = pawn & RANK_2_MASK;

	if (secondRank.IsEmpty()) return;

	Bitboard northNorthMove = ShiftNorth(northMove) & emptySquares;
	if (!northNorthMove.IsEmpty()) {
		moves.push_back(Move{pawn, northNorthMove});
	}
}


void MoveGenerator::GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push, Bitboard enemyPieces) {
	Bitboard westCaptureMove = ShiftWest(push) & enemyPieces;
	if (!westCaptureMove.IsEmpty()) {
		moves.push_back(Move{pawn, westCaptureMove});
	}

	Bitboard eastCaptureMove = ShiftEast(push) & enemyPieces;
	if (!eastCaptureMove.IsEmpty()) {
		moves.push_back(Move{pawn, eastCaptureMove});
	}
}

void MoveGenerator::GenerateWhiteKnightMoves(std::vector<Move>& moves) {
	Bitboard knights = m_board.GetPieceBitboard(Piece::WHITE_KNIGHT);
	Bitboard viable = ~m_board.GetWhitePieceBitboard();

	while (!knights.IsEmpty()) {
		Bitboard knight = knights.PopLsb();

		GenerateKnightHops(moves, knight, viable);
	}
}

void MoveGenerator::GenerateKnightHops(std::vector<Move>& moves, Bitboard knight, Bitboard viable) {
	GenerateKnightHopsNorth(moves, knight, viable);
	GenerateKnightHopsEast(moves, knight, viable);
	GenerateKnightHopsSouth(moves, knight, viable);
	GenerateKnightHopsWest(moves, knight, viable);
}

void MoveGenerator::GenerateKnightHopsNorth(std::vector<Move>& moves, Bitboard knight, Bitboard viable) {
	Bitboard north = (knight << 16);

	if (north.IsEmpty()) return;

	Bitboard nnw = ShiftWest(north) & viable;
	if (!nnw.IsEmpty())
		moves.push_back(Move{knight, nnw});

	Bitboard nne = ShiftEast(north) & viable;
	if (!nne.IsEmpty())
		moves.push_back(Move{knight, nne});
}

void MoveGenerator::GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight, Bitboard viable) {
	Bitboard east = (knight & NON_FILE_G_OR_H_MASK) >> 2;

	if (east.IsEmpty()) return;

	Bitboard een = ShiftNorth(east) & viable;
	if (!een.IsEmpty())
		moves.push_back(Move{knight, een});
	
	Bitboard ees = ShiftSouth(east) & viable;
	if (!ees.IsEmpty())
		moves.push_back(Move{knight, ees});
}

void MoveGenerator::GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight, Bitboard viable) {
	Bitboard south = (knight >> 16);

	if (south.IsEmpty()) return;

	Bitboard ssw = ShiftWest(south) & viable;
	if (!ssw.IsEmpty())
		moves.push_back(Move{knight, ssw});

	Bitboard sse = ShiftEast(south) & viable;
	if (!sse.IsEmpty())
		moves.push_back(Move{knight, sse});
}

void MoveGenerator::GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight, Bitboard viable) {
	Bitboard west = (knight & NON_FILE_A_OR_B_MASK) << 2;

	if (west.IsEmpty()) return;

	Bitboard wwn = ShiftNorth(west) & viable;
	if (!wwn.IsEmpty())
		moves.push_back(Move{knight, wwn});
	
	Bitboard wws = ShiftSouth(west) & viable;
	if (!wws.IsEmpty())
		moves.push_back(Move{knight, wws});
}

void MoveGenerator::GenerateBlackMoves(std::vector<Move>& moves) {
}