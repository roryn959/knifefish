#pragma once

#include <cstdint>

#define CASTLE_PERMISSIONS_LIST 	\
	X(WHITE_KINGSIDE)				\
	X(WHITE_QUEENSIDE)				\
	X(BLACK_KINGSIDE)				\
	X(BLACK_QUEENSIDE)				

enum CastlePermission : uint8_t {
	WHITE_KINGSIDE 	= 1 << 0,
	WHITE_QUEENSIDE = 1 << 1,
	BLACK_KINGSIDE 	= 1 << 2,
	BLACK_QUEENSIDE = 1 << 3
};