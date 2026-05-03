#pragma once

#include "Config.h"

#include "BoardRepresentation/Board.h"
#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Pieces.h"

#include "Engine/Constants.h"
#include "Engine/Killers.h"
#include "Engine/MagicBitboardHelper.h"
#include "Engine/Move.h"
#include "Engine/MoveGenerator.h"
#include "Engine/MoveHistory.h"
#include "Engine/PrincipleVariation.h"
#include "Engine/TranspositionTable.h"
#include "Engine/Undo.h"


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

	int16_t RootNegamax(int8_t depth, int16_t alpha, int16_t beta, const Move& prevBestMove, Move& bestMove);
	int16_t Negamax(int8_t depth, int8_t ply, int16_t alpha, int16_t beta, bool nmp = false);

	int16_t Quiescence(int8_t ply, int16_t alpha, int16_t beta);

#if DEBUG
	void PrintPv(int8_t depth);
#endif

	int Perft(int8_t depth);

	Board& 					m_board;
	MoveGenerator 			m_moveGenerator;
	TranspositionTable 		m_transpositionTable;
	Killers					m_killers;
	MoveHistory				m_moveHistory;
#if DEBUG
	PrincipleVariation		m_principleVariation;
#endif

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