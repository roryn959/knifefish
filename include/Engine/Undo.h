#pragma once

#include <array>

#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Types.h"

struct Undo {
	Piece m_capturedPiece;
	Square m_enPassantSquare;
	bool m_isPromotion; // Do we ever use this??
	std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> m_castlePermissions;
};