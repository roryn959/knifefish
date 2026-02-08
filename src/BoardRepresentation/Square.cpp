#include "BoardRepresentation/Square.h"

std::ostream& operator<<(std::ostream& os, Square sq) {
	switch (sq) {
		#define X(square) case (Square::square): { os << #square; break; }
		SQUARE_LIST
		#undef X

		default:
			break;
	}

	return os;
}