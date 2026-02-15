#include "BoardRepresentation/Square.h"

std::ostream& operator<<(std::ostream& os, Square sq) {
	return os << SquareToString(sq);
}