#pragma once

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
#include "Engine/Undo.h"


constexpr int PIECE_VALUES[] = { 1, 3, 3, 5, 9, 1000, -1, -3, -3, -5, -9, -1000 };

class Player {
public:
	Player(Board& board);

	Move GoDepth(int depth);

	int Evaluate();

	Move RootNegamax(int depth);
	int Negamax(int depth, int colour);

private:
	Board& m_board;
	MoveGenerator m_moveGenerator;

#if DEBUG
	int m_nodesSearched;
#endif
};