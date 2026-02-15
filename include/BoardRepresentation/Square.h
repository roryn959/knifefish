#pragma once

#include <cstdint>
#include <iostream>

// This gives a mapping from traditional notation (e5) to index on a bitboard (35?)
// It starts with h1 because 0 is the bottom-right square, then we move up right-to-left rankwise.
#define SQUARE_LIST										\
	X(h1) X(g1) X(f1) X(e1) X(d1) X(c1) X(b1) X(a1)		\
	X(h2) X(g2) X(f2) X(e2) X(d2) X(c2) X(b2) X(a2)		\
	X(h3) X(g3) X(f3) X(e3) X(d3) X(c3) X(b3) X(a3)		\
	X(h4) X(g4) X(f4) X(e4) X(d4) X(c4) X(b4) X(a4)		\
	X(h5) X(g5) X(f5) X(e5) X(d5) X(c5) X(b5) X(a5)		\
	X(h6) X(g6) X(f6) X(e6) X(d6) X(c6) X(b6) X(a6)		\
	X(h7) X(g7) X(f7) X(e7) X(d7) X(c7) X(b7) X(a7)		\
	X(h8) X(g8) X(f8) X(e8) X(d8) X(c8) X(b8) X(a8)		

#define X(square) square,
enum class Square : uint8_t {
	SQUARE_LIST
	NUMBER_OF_SQUARES
};
#undef X

inline std::string SquareToString(Square sq) {
	switch (sq) {
		#define X(square) case (Square::square) : { return #square; }
		SQUARE_LIST
		#undef X

		default:
			return "INVALID_SQUARE";
	}
}

std::ostream& operator<<(std::ostream& os, Square sq);

constexpr inline Square& operator++(Square& sq) {
	sq = static_cast<Square>(
		static_cast<uint8_t>(sq) + 1
	);
	return sq;
}

constexpr inline uint64_t GetBitmask(Square sq) {
	return 1ULL << static_cast<uint8_t>(sq);
}

constexpr inline Square GetSquareFromBitmask(uint64_t bb) {
	return static_cast<Square>(__builtin_ctzll(bb));
}