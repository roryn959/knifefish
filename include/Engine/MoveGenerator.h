#pragma once

#include <vector>

#include "BoardRepresentation/Board.h"
#include "Engine/Move.h"


class MoveGenerator {
public:
	MoveGenerator(const Board& board);

	std::vector<Move> 	GenerateMoves();

private:
	void PrepareWhiteMoveGeneration();
	void PrepareBlackMoveGeneration();

	void GenerateAttackSet();
	void GeneratePawnAttackSet();
	void GenerateKnightAttackSet();
	void GenerateBishopAttackSet();
	void GenerateRookAttackSet();
	void GenerateQueenAttackSet();
	void GenerateKingAttackSet();

	std::vector<Move> GeneratePseudoMoves();

	void GenerateWhitePawnMoves(std::vector<Move>& moves);
	void GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push);
	void GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push);

	void GenerateBlackPawnMoves(std::vector<Move>& moves);
	void GenerateBlackPawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push);
	void GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push);

	void GenerateKnightMoves(std::vector<Move>& moves);
	void GenerateKnightHopsNorth(std::vector<Move>& moves, Bitboard knight);
	void GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight);
	void GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight);
	void GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight);

	void GeneratePieceSlideNorthEast(std::vector<Move>& moves, Bitboard piece);
	void GeneratePieceSlideSouthEast(std::vector<Move>& moves, Bitboard piece);
	void GeneratePieceSlideSouthWest(std::vector<Move>& moves, Bitboard piece);
	void GeneratePieceSlideNorthWest(std::vector<Move>& moves, Bitboard piece);

	void GeneratePieceSlideNorth(std::vector<Move>& moves, Bitboard piece);
	void GeneratePieceSlideEast(std::vector<Move>& moves, Bitboard piece);
	void GeneratePieceSlideSouth(std::vector<Move>& moves, Bitboard piece);
	void GeneratePieceSlideWest(std::vector<Move>& moves, Bitboard piece);

	void GenerateKingNorthMove(std::vector<Move>& moves);
	void GenerateKingEastMoves(std::vector<Move>& moves);
	void GenerateKingSouthMove(std::vector<Move>& moves);
	void GenerateKingWestMoves(std::vector<Move>& moves);

	void GenerateBishopMoves(std::vector<Move>& moves);
	void GenerateRookMoves(std::vector<Move>& moves);
	void GenerateQueenMoves(std::vector<Move>& moves);
	void GenerateKingMoves(std::vector<Move>& moves);

	void GenerateWhiteCastleMoves(std::vector<Move>& moves);
	void GenerateBlackCastleMoves(std::vector<Move>& moves);

	const Board& m_board;

	Bitboard m_friendlyPieces;
	Bitboard m_enemyPieces;

	Bitboard m_occupiedSquares;
	Bitboard m_emptySquares;

	Bitboard m_attackSet;

	Bitboard m_pawns;
	Bitboard m_knights;
	Bitboard m_bishops;
	Bitboard m_rooks;
	Bitboard m_queens;
	Bitboard m_king;
};