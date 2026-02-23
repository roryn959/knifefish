#pragma once

#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "Config.h"

#include "BoardRepresentation/Board.h"
#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Pieces.h"

#include "Engine/Move.h"
#include "Engine/MoveGenerator.h"
#include "Engine/TranspositionTable.h"
#include "Engine/Undo.h"


constexpr int16_t MATE_SCORE { 30'000 };
constexpr int16_t MAX_SCORE { 32'000 };
constexpr std::array<int, 12> PIECE_VALUES = { 100, 320, 330, 500, 900, 10000, -100, -320, -330, -500, -900, -10000 };

constexpr std::array<int, 64> FlippedPst(const std::array<int, static_cast<size_t>(Square::COUNT)>& pst) {
	std::array<int, static_cast<size_t>(Square::COUNT)> flipped;

	for (int i = 0; i < static_cast<size_t>(Square::COUNT); ++i)
		flipped[i] = -pst[i ^ 56];
	
	return flipped;
}

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_PAWN_PST = {
	0,  0,  0,  0,  0,  0,  0,  0,
	50, 50, 50, 50, 50, 50, 50, 50,
	10, 10, 20, 30, 30, 20, 10, 10,
	5,  5, 10, 25, 25, 10,  5,  5,
	0,  0,  0, 20, 20,  0,  0,  0,
	5, -5,-10,  0,  0,-10, -5,  5,
	5, 10, 10,-20,-20, 10, 10,  5,
	0,  0,  0,  0,  0,  0,  0,  0
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_KNIGHT_PST = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_BISHOP_PST = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_ROOK_PST = {
	 0,  0,  0,  0,  0,  0,  0,  0,
	5, 10, 10, 10, 10, 10, 10,  5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	0,  0,  0,  5,  5,  0,  0,  0
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_QUEEN_PST = {
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-5,  0,  5,  5,  5,  5,  0, -5,
	0,  0,  5,  5,  5,  5,  0, -5,
	-10,  5,  5,  5,  5,  5,  0,-10,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_KING_PST = {
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	20, 20,  0,  0,  0,  0, 20, 20,
	20, 30, 10,  0,  0, 10, 30, 20
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_PAWN_PST = FlippedPst(BLACK_PAWN_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_KNIGHT_PST = FlippedPst(BLACK_KNIGHT_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_BISHOP_PST = FlippedPst(BLACK_BISHOP_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_ROOK_PST = FlippedPst(BLACK_ROOK_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_QUEEN_PST = FlippedPst(BLACK_QUEEN_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_KING_PST = FlippedPst(BLACK_KING_PST);

constexpr std::array<std::array<int, static_cast<size_t>(Square::COUNT)>, Piece::NUM_PIECES> pieceSquareTables = {
	WHITE_PAWN_PST,
	WHITE_KNIGHT_PST,
	WHITE_BISHOP_PST,
	WHITE_ROOK_PST,
	WHITE_QUEEN_PST,
	WHITE_KING_PST,
	BLACK_PAWN_PST,
	BLACK_KNIGHT_PST,
	BLACK_BISHOP_PST,
	BLACK_ROOK_PST,
	BLACK_QUEEN_PST,
	BLACK_KING_PST
};

class Player {
public:
	Player(Board& board);

	int16_t Evaluate();
	Move GoDepth(int8_t depth);
	int RootPerft(int8_t depth);

private:
	bool IsCheckmate();

	Move IterativeDeepening(int8_t maxDepth);

	Move RootNegamax(int8_t depth, const Move& movePv);
	int16_t Negamax(int8_t depth, int16_t alpha, int16_t beta);
	int Perft(int8_t depth);

#if DEBUG
	int m_nodesSearched;
	int m_transpositionsHit;
#endif

	Board& m_board;
	MoveGenerator m_moveGenerator;
	TranspositionTable m_transpositionTable;
};