#include "Engine/Move.h"

std::ostream& operator<<(std::ostream& os, Move move) {
	os << GetSquareFromBitmask(move.m_from.GetBoard()) << GetSquareFromBitmask(move.m_to.GetBoard()) << '\n';
	return os;
}