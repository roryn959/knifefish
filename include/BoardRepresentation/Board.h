#pragma once

#include <iostream>

#include "Pieces.h"


class Board {
public:
	Board();

	inline Bitboard GetPieceBitboard(Piece p) 	const { return m_pieceBitboards[p]; }

	Bitboard GetAllPieceBitboard() const;
	Bitboard GetWhitePieceBitboard() const;
	Bitboard GetBlackPieceBitboard() const;

	inline bool IsWhiteTurn() const { return m_isWhiteTurn; }

	friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
	Bitboard m_pieceBitboards[Piece::NUM_PIECES];

	bool	m_isWhiteTurn;
};