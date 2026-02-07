#pragma once

#include <iostream>

#include "Pieces.h"


class Board {
public:
	Board();

	inline Bitboard GetBitBoard(Piece p) { return m_bitboards[p]; }

	friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
	Bitboard m_bitboards[Piece::NUM_PIECES];
};