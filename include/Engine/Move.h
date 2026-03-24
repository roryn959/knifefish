#pragma once

#include <iostream>
#include <string>

#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Pieces.h"


struct Move {
	int			m_score;
	Square		m_from;
	Square		m_to;
	Piece		m_promotionPiece;
	bool 		m_isCapture;
	bool		m_isDoublePawnPush;
	bool		m_isEnPassant;
	bool		m_isCastle;

	inline bool operator==(const Move& rhs) const noexcept {
		return m_from == rhs.m_from
			&& m_to == rhs.m_to
			&& m_promotionPiece == rhs.m_promotionPiece
			&& m_isCapture == rhs.m_isCapture
			&& m_isDoublePawnPush == rhs.m_isDoublePawnPush
			&& m_isEnPassant == rhs.m_isEnPassant
			&& m_isCastle == rhs.m_isCastle;
	}

	std::string ToString() const;
};

std::ostream& operator<<(std::ostream& os, Move move);