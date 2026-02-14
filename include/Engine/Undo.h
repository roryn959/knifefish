#pragma once

#include "BoardRepresentation/Pieces.h"

struct Undo {
	Piece m_capturedPiece;
	Bitboard m_enPassantSquare;
	bool m_isPromotion;
	bool m_isWhiteKingsideCastlePermitted;
	bool m_isWhiteQueensideCastlePermitted;
	bool m_isBlackKingsideCastlePermitted;
	bool m_isBlackQueensideCastlePermitted;
};