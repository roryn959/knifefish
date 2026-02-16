#include "BoardRepresentation/Bitboard.h"


std::ostream& operator<<(std::ostream& os, const Bitboard& board) {
	char boardArray[64];
	for (int i = 0; i < 64; ++i) {
		Bitboard bitPosition = 1ULL << i;

		if ((board & bitPosition).Any()) 
			boardArray[i] = 'X';
		else
			boardArray[i] = 'O';
	}

	int col = 0;
	for (int i = 63; i >= 0; --i) {
		os << boardArray[i] << ' ';
		if (++col == 8) {
			std::cout << '\n';
			col = 0;
		}
	}

	os << '\n';

	return os;
}