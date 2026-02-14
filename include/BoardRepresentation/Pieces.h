#pragma once

#include "Bitboard.h"


#define PIECES_LIST 		\
	X(WHITE_PAWN)			\
	X(WHITE_KNIGHT)			\
	X(WHITE_BISHOP)			\
	X(WHITE_ROOK)			\
	X(WHITE_QUEEN)			\
	X(WHITE_KING)			\
	X(BLACK_PAWN)			\
	X(BLACK_KNIGHT)			\
	X(BLACK_BISHOP)			\
	X(BLACK_ROOK)			\
	X(BLACK_QUEEN)			\
	X(BLACK_KING)

#define WHITE_PIECES_LIST	\
	X(WHITE_PAWN)			\
	X(WHITE_KNIGHT)			\
	X(WHITE_BISHOP)			\
	X(WHITE_ROOK)			\
	X(WHITE_QUEEN)			\
	X(WHITE_KING)

#define BLACK_PIECES_LIST	\
	X(BLACK_PAWN)			\
	X(BLACK_KNIGHT)			\
	X(BLACK_BISHOP)			\
	X(BLACK_ROOK)			\
	X(BLACK_QUEEN)			\
	X(BLACK_KING)

#define X(piece) piece,
enum Piece : uint8_t {
	PIECES_LIST
	NUM_PIECES,
	EMPTY
};
#undef X

constexpr inline char GetChar(Piece p) {
	switch (p) {
		case (Piece::WHITE_PAWN): 	return 'P';
		case (Piece::WHITE_KNIGHT): return 'N';
		case (Piece::WHITE_BISHOP): return 'B';
		case (Piece::WHITE_ROOK):	return 'R';
		case (Piece::WHITE_QUEEN): 	return 'Q';
		case (Piece::WHITE_KING): 	return 'K';
		case (Piece::BLACK_PAWN): 	return 'p';
		case (Piece::BLACK_KNIGHT): return 'n';
		case (Piece::BLACK_BISHOP): return 'b';
		case (Piece::BLACK_ROOK):	return 'r';
		case (Piece::BLACK_QUEEN): 	return 'q';
		case (Piece::BLACK_KING): 	return 'k';
		case (Piece::EMPTY):		return '.';
		default:					return 'X';
	}
}

constexpr inline bool IsEmpty(Piece p) { return p == Piece::EMPTY; }
constexpr inline bool IsWhite(Piece p) { return Piece::WHITE_PAWN <= p && p <= Piece::WHITE_KING; }
constexpr inline bool IsBlack(Piece p) { return Piece::BLACK_PAWN <= p && p <= Piece::BLACK_KING; }

template<Piece P>
constexpr Bitboard GetStartingPositionBitboard();
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::WHITE_PAWN>() { return RANK_2_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::WHITE_KNIGHT>() { return B1_MASK | G1_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::WHITE_BISHOP>() { return C1_MASK | F1_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::WHITE_ROOK>() { return A1_MASK | H1_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::WHITE_QUEEN>() { return D1_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::WHITE_KING>() { return E1_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::BLACK_PAWN>() { return RANK_7_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::BLACK_KNIGHT>() { return B8_MASK | G8_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::BLACK_BISHOP>() { return C8_MASK | F8_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::BLACK_ROOK>() { return A8_MASK | H8_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::BLACK_QUEEN>() { return D8_MASK; }
template<>
constexpr Bitboard GetStartingPositionBitboard<Piece::BLACK_KING>() { return E8_MASK; }

constexpr Bitboard GetStartingWhitePiecesBitboard() { 
	Bitboard bitboard{0ULL};

	#define X(piece) bitboard |= GetStartingPositionBitboard<Piece::piece>();
	WHITE_PIECES_LIST
	#undef X

	return bitboard;
 }

 constexpr Bitboard GetStartingBlackPiecesBitboard() { 
	Bitboard bitboard{0ULL};

	#define X(piece) bitboard |= GetStartingPositionBitboard<Piece::piece>();
	BLACK_PIECES_LIST
	#undef X

	return bitboard;
 }

 constexpr Bitboard GetStartingAllPiecesBitboard() { 
	Bitboard bitboard{0ULL};

	#define X(piece) bitboard |= GetStartingPositionBitboard<Piece::piece>();
	PIECES_LIST
	#undef X

	return bitboard;
 }