#pragma once

#include <iostream>

#include "BoardRepresentation/Bitboard.h"

struct Move {
	Bitboard m_from;
	Bitboard m_to;
};

std::ostream& operator<<(std::ostream& os, Move move);