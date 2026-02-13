#pragma once

#include <iostream>

#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Pieces.h"


struct Move {
	Bitboard 	m_from;
	Bitboard 	m_to;
	Piece		m_promotionPiece;
	bool 		m_isCapture;
	bool		m_isDoublePawnPush;
	bool		m_isEnPassant;
	bool		m_isCastle;
};

std::ostream& operator<<(std::ostream& os, Move move);