#pragma once

#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>

#include "Config.h"

#include "BoardRepresentation/Board.h"
#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Pieces.h"

#include "Engine/Move.h"
#include "Engine/MoveGenerator.h"
#include "Engine/TranspositionTable.h"
#include "Engine/Undo.h"


using Clock = std::chrono::steady_clock;
using Moment = Clock::time_point;
using ms = std::chrono::milliseconds;

constexpr double MillisecsToSecs(int ms) { return ms / 1000.0; }
constexpr int SecsToMillisecs(double secs) { return round(secs * 1000.0); }

#define TIME_CHECK_FREQUENCY 2048

#define MAX_DEPTH 20
#define DELTA_PRUNE_MARGIN 200

constexpr int16_t MATE_SCORE { 30'000 };
constexpr int16_t MAX_SCORE { 32'000 };

constexpr std::array<int, 12> PIECE_VALUES = { 100, 320, 330, 500, 900, 10000, -100, -320, -330, -500, -900, -10000 };

constexpr std::array<int, static_cast<size_t>(Piece::NUM_PIECES)> MG_PIECE_VALUES { 82, 337, 365, 477, 1025, 10000, -82, -337, -365, -477, -1025, -10000 };
constexpr std::array<int, static_cast<size_t>(Piece::NUM_PIECES)> EG_PIECE_VALUES { 94, 281, 297, 512, 936, 10000, -94, -281, -297, -512, -936, -10000 };

constexpr std::array<int, 64> FlippedPst(const std::array<int, static_cast<size_t>(Square::COUNT)>& pst) {
	std::array<int, static_cast<size_t>(Square::COUNT)> flipped;

	for (int i = 0; i < static_cast<size_t>(Square::COUNT); ++i)
		flipped[i] = -pst[i ^ 56];
	
	return flipped;
}

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_PAWN_MG_PST = {
	 0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_PAWN_EG_PST = {
	  0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_KNIGHT_MG_PST = {
	-167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_KNIGHT_EG_PST = {
	-58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_BISHOP_MG_PST = {
	-29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_BISHOP_EG_PST = {
	-14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_ROOK_MG_PST = {
	 32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_ROOK_EG_PST = {
	13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_QUEEN_MG_PST = {
	-28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_QUEEN_EG_PST = {
	 -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_KING_MG_PST = {
	-65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> BLACK_KING_EG_PST = {
	-74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_PAWN_MG_PST = FlippedPst(BLACK_PAWN_MG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_KNIGHT_MG_PST = FlippedPst(BLACK_KNIGHT_MG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_BISHOP_MG_PST = FlippedPst(BLACK_BISHOP_MG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_ROOK_MG_PST = FlippedPst(BLACK_ROOK_MG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_QUEEN_MG_PST = FlippedPst(BLACK_QUEEN_MG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_KING_MG_PST = FlippedPst(BLACK_KING_MG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_PAWN_EG_PST = FlippedPst(BLACK_PAWN_EG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_KNIGHT_EG_PST = FlippedPst(BLACK_KNIGHT_EG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_BISHOP_EG_PST = FlippedPst(BLACK_BISHOP_EG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_ROOK_EG_PST = FlippedPst(BLACK_ROOK_EG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_QUEEN_EG_PST = FlippedPst(BLACK_QUEEN_EG_PST);
constexpr std::array<int, static_cast<size_t>(Square::COUNT)> WHITE_KING_EG_PST = FlippedPst(BLACK_KING_EG_PST);

constexpr std::array<std::array<int, static_cast<size_t>(Square::COUNT)>, Piece::NUM_PIECES> MG_PST_LIST = {
	WHITE_PAWN_MG_PST,
	WHITE_KNIGHT_MG_PST,
	WHITE_BISHOP_MG_PST,
	WHITE_ROOK_MG_PST,
	WHITE_QUEEN_MG_PST,
	WHITE_KING_MG_PST,
	BLACK_PAWN_MG_PST,
	BLACK_KNIGHT_MG_PST,
	BLACK_BISHOP_MG_PST,
	BLACK_ROOK_MG_PST,
	BLACK_QUEEN_MG_PST,
	BLACK_KING_MG_PST
};

constexpr std::array<std::array<int, static_cast<size_t>(Square::COUNT)>, Piece::NUM_PIECES> EG_PST_LIST = {
	WHITE_PAWN_EG_PST,
	WHITE_KNIGHT_EG_PST,
	WHITE_BISHOP_EG_PST,
	WHITE_ROOK_EG_PST,
	WHITE_QUEEN_EG_PST,
	WHITE_KING_EG_PST,
	BLACK_PAWN_EG_PST,
	BLACK_KNIGHT_EG_PST,
	BLACK_BISHOP_EG_PST,
	BLACK_ROOK_EG_PST,
	BLACK_QUEEN_EG_PST,
	BLACK_KING_EG_PST
};

class Killers {
public:
	inline constexpr Killers() noexcept : m_killers{} { Reset(); }
	inline constexpr void Reset() noexcept { m_killers.fill(Move{ QUIET_MOVE_BASE_SCORE, Square::NONE, Square::NONE }); };

	inline const Move& GetFirst(int8_t depth) noexcept { return m_killers[FirstIndex(depth)]; }
	inline const Move& GetSecond(int8_t depth) noexcept { return m_killers[SecondIndex(depth)]; }

	inline void Set(int8_t depth, Move move) noexcept {
		if (move == m_killers[FirstIndex(depth)]) return;

		std::swap(m_killers[FirstIndex(depth)], m_killers[SecondIndex(depth)]);
		m_killers[FirstIndex(depth)] = move;
	}

private:
	inline size_t FirstIndex(int8_t depth) noexcept { return depth * 2; }
	inline size_t SecondIndex(int8_t depth) noexcept { return (depth * 2) + 1; }

	std::array<Move, MAX_DEPTH * 2> m_killers;
};

class Player {
public:
	Player(Board& board);

	int16_t Evaluate();

	Move Go(int depth, int wtime, int btime, int winc, int binc, int movestogo, int movetime);

	int RootPerft(int8_t depth);

	inline void ClearTranspositionTable() { m_transpositionTable.Clear(); }

private:
	void InitialisePieceSquareTables();

	Move IterativeDeepening(int8_t maxDepth);

	int16_t RootNegamax(int8_t depth, const Move& movePv, Move& bestMove);
	int16_t Negamax(int8_t depth, int16_t alpha, int16_t beta);

	int16_t Quiescence(int16_t alpha, int16_t beta);

	int Perft(int8_t depth);

	Board& 					m_board;
	MoveGenerator 			m_moveGenerator;
	TranspositionTable 		m_transpositionTable;
	Killers					m_killers;

	int 	m_nodesSearched;

	Moment 	m_deadline;
	bool 	m_isStopped;

	std::array<std::array<int, static_cast<size_t>(Square::COUNT)>, static_cast<size_t>(Piece::NUM_PIECES)> m_midgamePieceSquareTables;
	std::array<std::array<int, static_cast<size_t>(Square::COUNT)>, static_cast<size_t>(Piece::NUM_PIECES)> m_endgamePieceSquareTables;


#if DEBUG
	int 	m_transpositionsHit;
	int 	m_currentDepthNodes;
	int 	m_quiescenceNodesSearched;
#endif
};