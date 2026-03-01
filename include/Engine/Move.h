#pragma once

#include <iostream>
#include <string>

#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Pieces.h"


struct Move {
	Square		m_from;
	Square		m_to;
	Piece		m_promotionPiece;
	bool 		m_isCapture;
	bool		m_isDoublePawnPush;
	bool		m_isEnPassant;
	bool		m_isCastle;

	inline bool operator==(const Move& rhs) noexcept { return ToString() == rhs.ToString(); }

	std::string ToString() const;
};

std::ostream& operator<<(std::ostream& os, Move move);