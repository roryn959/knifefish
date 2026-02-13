#pragma once

#include <vector>

#include "BoardRepresentation/Board.h"
#include "Engine/Move.h"

constexpr Bitboard RANK_2_MASK				{ 0x000000000000FF00ULL };
constexpr Bitboard RANK_7_MASK				{ 0x00FF000000000000ULL };

constexpr Bitboard NON_FILE_A_MASK			{ 0x7F7F7F7F7F7F7F7FULL };
constexpr Bitboard NON_FILE_A_OR_B_MASK		{ 0x3F3F3F3F3F3F3F3FULL };
constexpr Bitboard NON_FILE_G_OR_H_MASK		{ 0xFCFCFCFCFCFCFCFCULL };
constexpr Bitboard NON_FILE_H_MASK			{ 0xFEFEFEFEFEFEFEFEULL };

class MoveGenerator {
public:
	MoveGenerator(const Board& board);

	std::vector<Move> 	GenerateMoves();

	void				GenerateWhiteMoves(std::vector<Move>& moves);
	void				GenerateBlackMoves(std::vector<Move>& moves);

	void				GenerateWhitePawnMoves(std::vector<Move>& moves);
	void				GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push);
	void				GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push);

	void				GenerateBlackPawnMoves(std::vector<Move>& moves);
	void				GenerateBlackPawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push);
	void				GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push);

	void				GenerateKnightMoves(std::vector<Move>& moves);
	void				GenerateKnightHopsNorth(std::vector<Move>& moves, Bitboard knight);
	void				GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight);
	void				GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight);
	void				GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight);

	void				GeneratePieceSlideNorthEast(std::vector<Move>& moves, Bitboard piece);
	void				GeneratePieceSlideSouthEast(std::vector<Move>& moves, Bitboard piece);
	void				GeneratePieceSlideSouthWest(std::vector<Move>& moves, Bitboard piece);
	void				GeneratePieceSlideNorthWest(std::vector<Move>& moves, Bitboard piece);

	void				GeneratePieceSlideNorth(std::vector<Move>& moves, Bitboard piece);
	void				GeneratePieceSlideEast(std::vector<Move>& moves, Bitboard piece);
	void				GeneratePieceSlideSouth(std::vector<Move>& moves, Bitboard piece);
	void				GeneratePieceSlideWest(std::vector<Move>& moves, Bitboard piece);

	void				GenerateKingNorthMove(std::vector<Move>& moves);
	void				GenerateKingEastMoves(std::vector<Move>& moves);
	void				GenerateKingSouthMove(std::vector<Move>& moves);
	void				GenerateKingWestMoves(std::vector<Move>& moves);

	void				GenerateBishopMoves(std::vector<Move>& moves);
	void				GenerateRookMoves(std::vector<Move>& moves);
	void				GenerateQueenMoves(std::vector<Move>& moves);
	void				GenerateKingMoves(std::vector<Move>&moves);

	// Note: We should not need rank masks here because shifting should throw away invalid vertical moves anyway.
	constexpr inline Bitboard ShiftNorth(Bitboard bitboard) const noexcept { return bitboard <<= 8; }
	constexpr inline Bitboard ShiftEast(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_H_MASK; return bitboard >>= 1; }
	constexpr inline Bitboard ShiftSouth(Bitboard bitboard) const noexcept { return bitboard >>= 8; }
	constexpr inline Bitboard ShiftWest(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_A_MASK; return bitboard <<= 1; }
	constexpr inline Bitboard ShiftNorthEast(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_H_MASK; return bitboard <<= 7; }
	constexpr inline Bitboard ShiftSouthEast(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_H_MASK; return bitboard >>= 9; }
	constexpr inline Bitboard ShiftSouthWest(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_A_MASK; return bitboard >>= 7; }
	constexpr inline Bitboard ShiftNorthWest(Bitboard bitboard) const noexcept { bitboard &= NON_FILE_A_MASK; return bitboard <<= 9; }

private:
	const Board& m_board;

	Bitboard m_friendlyPieces;
	Bitboard m_enemyPieces;

	Bitboard m_emptySquares;

	Bitboard m_pawns;
	Bitboard m_knights;
	Bitboard m_bishops;
	Bitboard m_rooks;
	Bitboard m_queens;
	Bitboard m_king;
};