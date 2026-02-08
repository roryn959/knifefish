#pragma once

#include <vector>

#include "BoardRepresentation/Board.h"
#include "Engine/Move.h"

constexpr Bitboard RANK_2_MASK				{ 0x000000000000FF00ULL };

constexpr Bitboard NON_FILE_A_MASK			{ 0x7F7F7F7F7F7F7F7FULL };
constexpr Bitboard NON_FILE_A_OR_B_MASK		{ 0x3F3F3F3F3F3F3F3FULL };
constexpr Bitboard NON_FILE_G_OR_H_MASK		{ 0xFCFCFCFCFCFCFCFCULL };
constexpr Bitboard NON_FILE_H_MASK			{ 0xFEFEFEFEFEFEFEFEULL };

class MoveGenerator {
public:
	MoveGenerator(const Board& board);

	std::vector<Move> 	GenerateMoves();

	void				GenerateWhiteMoves(std::vector<Move>& moves);

	void				GenerateWhitePawnMoves(std::vector<Move>& moves);
	void				GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push, Bitboard emptySquares);
	void				GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push, Bitboard enemyPieces);

	void				GenerateWhiteKnightMoves(std::vector<Move>& moves);

	void				GenerateKnightHops(std::vector<Move>& moves, Bitboard knight, Bitboard viable);
	void				GenerateKnightHopsNorth(std::vector<Move>& moves, Bitboard knight, Bitboard viable);
	void				GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight, Bitboard viable);
	void				GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight, Bitboard viable);
	void				GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight, Bitboard viable);

	void				GenerateBlackMoves(std::vector<Move>& moves);

	// Note: We should not need rank masks here because shifting should throw away invalid vertical moves anyway.
	constexpr inline Bitboard ShiftNorth(Bitboard bitboard) const noexcept { return bitboard <<= 8; }
	constexpr inline Bitboard ShiftEast(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_H_MASK; return bitboard >>= 1; }
	constexpr inline Bitboard ShiftSouth(Bitboard bitboard) const noexcept { return bitboard >>= 8; }
	constexpr inline Bitboard ShiftWest(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_A_MASK; return bitboard <<= 1; }

private:
	const Board& m_board;
};