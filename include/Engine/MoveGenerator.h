#pragma once

#include <array>

#include "BoardRepresentation/Board.h"
#include "Engine/MagicBitboardHelper.h"
#include "Engine/Move.h"

#define PV_MOVE_BASE_SCORE 25'000
#define TT_MOVE_BASE_SCORE 20'000
#define CAPTURE_BASE_SCORE 15'000
#define PROMOTION_BASE_SCORE 10'000
#define FIRST_KILLER_BASE_SCORE 8'000
#define SECOND_KILLER_BASE_SCORE 7'000
#define QUIET_MOVE_BASE_SCORE 0

constexpr std::array<int, 12> ABSOLUTE_PIECE_VALUES = { 100, 320, 330, 500, 900, 10000, 100, 320, 330, 500, 900, 10000 };


class MoveList {
public:
	static constexpr size_t MAX_POSSIBLE_MOVES = 218;

	inline MoveList() : m_moveList{}, m_numMoves{0} {}

	Move* begin() 	{ return m_moveList.data(); }
	Move* end() 	{ return m_moveList.data() + m_numMoves; }

	const Move* begin() const { return m_moveList.data(); }
	const Move* end() 	const { return m_moveList.data() + m_numMoves; }

	inline Move& operator[](size_t i) { return m_moveList[i]; }
	inline const Move& operator[](size_t i) const { return m_moveList[i]; }

	inline size_t size() const { return m_numMoves; }

	inline void clear() { m_numMoves = 0; }

	inline void push_back(const Move& move) { m_moveList[m_numMoves++] = move; }

private:
	std::array<Move, MAX_POSSIBLE_MOVES> m_moveList;
	size_t m_numMoves;
};

struct MoveGenerationContext {
	Bitboard													m_friendlyPawnBB;
	Bitboard 													m_friendlyKnightBB;
	Bitboard 													m_friendlyBishopBB;
	Bitboard 													m_friendlyRookBB;
	Bitboard 													m_friendlyQueenBB;
	Bitboard 													m_friendlyKingBB;
	Square														m_friendlyKingSquare;
	Bitboard													m_friendlyPieceBB;

	Bitboard 													m_enemyPawnBB;
	Bitboard 													m_enemyKnightBB;
	Bitboard 													m_enemyBishopBB;
	Bitboard 													m_enemyRookBB;
	Bitboard 													m_enemyQueenBB;
	Bitboard 													m_enemyKingBB;

	Square														m_enPassantSquare;
	Bitboard 													m_allPieceBB;
	Bitboard 													m_emptySquareBB;
	Bitboard 													m_enemyPieceBB;
	Bitboard 													m_enemyAttackSet;
	Bitboard 													m_checkMaskBB;
	std::array<Bitboard, static_cast<size_t>(Square::COUNT)> 	m_pinMasks;
	Bitboard													m_checkerBB;
};

struct MoveGenerationParameters {
	MoveList&													m_moves;
	bool														m_capturesOnly;
};

class MoveGenerator {
public:
	MoveGenerator(Board& board);

	MoveGenerationContext GetMoveGenerationContext() const;

	bool GenerateMoves(const MoveGenerationParameters& params) const;
	bool GenerateMoves(const MoveGenerationParameters& params, MoveGenerationContext& context) const;

	Bitboard GetAttackSet(Bitboard pawnBB, Bitboard knightBB, Bitboard bishopBB, Bitboard rookBB, Bitboard queenBB, Bitboard kingBB, Bitboard emptySquareBB, Bitboard allPieceBB) const;

	bool IsCheck() const;
	bool IsCheck(const MoveGenerationContext& context) const;

private:
	void GeneratePawnMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;
	void GenerateWhitePawnMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;
	void GenerateBlackPawnMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;

	void GenerateKnightMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;

	void GenerateBishopMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;

	void GenerateRookMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;

	void GenerateQueenMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;

	void GenerateKingMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;

	void GenerateCastleMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;
	void GenerateWhiteCastleMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;
	void GenerateBlackCastleMoves(const MoveGenerationParameters& params, const MoveGenerationContext& context) const;

	Bitboard GetWhitePawnAttackSet(Bitboard pawns) const;
	Bitboard GetBlackPawnAttackSet(Bitboard pawns) const;
	Bitboard GetKnightAttackSet(Bitboard knights) const;
	Bitboard GetBishopAttackSet(Bitboard bishops, Bitboard allPieces) const;
	Bitboard GetRookAttackSet(Bitboard rooks, Bitboard allPieces) const;
	Bitboard GetQueenAttackSet(Bitboard queens, Bitboard allPieces) const;
	Bitboard GetKingAttackSet(Bitboard king) const;

	Board& m_board;
	const MagicBitboardHelper m_magicBitboardHelper;
};