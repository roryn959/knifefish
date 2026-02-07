#include "Engine/Board.h"

Board::Board() {
	#define X(piece) m_bitboards[Piece::piece] = GetStartingPositionBitboard<Piece::piece>();
	PIECES_LIST
	#undef X
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
	Piece boardArray[64];

	for (int i = 0; i < 64; ++i) {
		boardArray[i] = Piece::EMPTY;

		Bitboard bitPosition = 1ULL << i;

		// if (bitPosition & board.m_bitboards[Piece::WHITE_PAWN])
		// 	boardArray[i] = Piece::WHITE_PAWN;
		// else if (bitPosition & board.m_bitboards[Piece::WHITE_KNIGHT])
		// 	boardArray[i] = Piece::WHITE_PAWN;
		#define X(piece) else if (bitPosition & board.m_bitboards[Piece::piece]) boardArray[i] = Piece::piece;
		if (false) ;
		PIECES_LIST
		#undef X
	}

	int col = 0;
	for (int i = 63; i >= 0; --i) {

		os << GetChar(boardArray[i]) << ' ';

		if (++col == 8) {
			os << '\n';
			col = 0;
		}
	}

	return os;
}