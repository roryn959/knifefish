#pragma once

#include <vector>

#include "BoardRepresentation/Board.h"
#include "Engine/MagicBitboardHelper.h"
#include "Engine/Move.h"


class MoveGenerator {
public:
	MoveGenerator(Board& board);

	std::vector<Move> GenerateLegalMoves();

	std::vector<Move> OldGenerateLegalMoves();
	std::vector<Move> GeneratePseudoMoves();

	bool IsAttackedByWhite(Bitboard squares);
	bool IsAttackedByBlack(Bitboard squares);

	Bitboard GetWhiteAttackSet();
	Bitboard GetBlackAttackSet();

private:

	std::vector<Move> FilterOutIllegalWhiteMoves(std::vector<Move>& moves);
	std::vector<Move> FilterOutIllegalBlackMoves(std::vector<Move>& moves);

	void PrepareWhiteMoveGeneration();
	void PrepareBlackMoveGeneration();

	void GenerateWhitePawnMoves(std::vector<Move>& moves);
	void GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push);
	void GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push);

	void GenerateBlackPawnMoves(std::vector<Move>& moves);
	void GenerateBlackPawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push);
	void GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push);

	void GenerateKnightMoves(std::vector<Move>& moves);
	void GenerateBishopMoves(std::vector<Move>& moves);
	void GenerateRookMoves(std::vector<Move>& moves);
	void GenerateQueenMoves(std::vector<Move>& moves);
	void GenerateKingMoves(std::vector<Move>& moves);

	void GenerateWhiteCastleMoves(std::vector<Move>& moves);
	void GenerateBlackCastleMoves(std::vector<Move>& moves);

	Bitboard GetWhitePawnAttackSet(Bitboard pawns);
	Bitboard GetBlackPawnAttackSet(Bitboard pawns);
	Bitboard GetKnightAttackSet(Bitboard knights);
	Bitboard GetBishopAttackSet(Bitboard bishops, Bitboard allPieces);
	Bitboard GetRookAttackSet(Bitboard rooks, Bitboard allPieces);
	Bitboard GetQueenAttackSet(Bitboard queens, Bitboard allPieces);
	Bitboard GetKingAttackSet(Bitboard king);

	Board& m_board;
	MagicBitboardHelper m_magicBitboardHelper;

	Bitboard m_friendlyPieces;
	Bitboard m_enemyPieces;

	Bitboard m_occupiedSquares;
	Bitboard m_emptySquares;

	Bitboard m_enemyAttackSet;

	Bitboard m_pawns;
	Bitboard m_knights;
	Bitboard m_bishops;
	Bitboard m_rooks;
	Bitboard m_queens;
	Bitboard m_king;
};