#pragma once

#include <array>

#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Types.h"

struct Undo {
	Piece m_capturedPiece;
	Bitboard m_enPassantSquare;
	bool m_isPromotion;
	std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> m_castlePermissions;
};