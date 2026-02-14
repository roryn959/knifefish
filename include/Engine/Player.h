#pragma once

#include <cstdlib>
#include <ctime>

#include "BoardRepresentation/Board.h"

#include "Engine/Move.h"
#include "Engine/MoveGenerator.h"


class Player {
public:
	Player(Board& board);

	Move GetMove();

private:
	Board& m_board;
	MoveGenerator m_moveGenerator;
};