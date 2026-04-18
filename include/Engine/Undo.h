#pragma once

#include <array>

#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Types.h"

struct Undo {
	Piece m_capturedPiece;
	Square m_enPassantSquare;
	uint8_t m_castlePermissions;
	size_t m_repetitionStackTail;
};