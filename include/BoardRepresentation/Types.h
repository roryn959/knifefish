#pragma once

#include <cstdint>

#define CASTLE_PERMISSIONS_LIST 	\
	X(WHITE_KINGSIDE)				\
	X(WHITE_QUEENSIDE)				\
	X(BLACK_KINGSIDE)				\
	X(BLACK_QUEENSIDE)

#define X(castle) castle,

enum class CastlePermission : uint8_t {
	CASTLE_PERMISSIONS_LIST
	COUNT
};

#undef X