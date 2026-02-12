#include "BoardRepresentation/Board.h"


Board::Board() :
	m_isWhiteTurn{false}
{
	#define X(piece) m_pieceBitboards[Piece::piece] = GetStartingPositionBitboard<Piece::piece>();
	PIECES_LIST
	#undef X
}

Bitboard Board::GetAllPieceBitboard() const {
	Bitboard allPieces{0ULL};
	
	#define X(piece) | m_pieceBitboards[Piece::piece]
	return allPieces PIECES_LIST;
	#undef X
}

Bitboard Board::GetWhitePieceBitboard() const {
	Bitboard whitePieces{0ULL};

	#define X(piece) | m_pieceBitboards[Piece::piece]
	return whitePieces WHITE_PIECES_LIST;
	#undef X
}

Bitboard Board::GetBlackPieceBitboard() const {
	Bitboard blackPieces{0ULL};

	#define X(piece) | m_pieceBitboards[Piece::piece]
	return blackPieces BLACK_PIECES_LIST;
	#undef X
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
	Piece boardArray[64];

	for (int i = 0; i < 64; ++i) {
		boardArray[i] = Piece::EMPTY;

		Bitboard bitPosition = 1ULL << i;

		#define X(piece) else if (!(bitPosition & board.GetPieceBitboard(Piece::piece)).IsEmpty()) boardArray[i] = Piece::piece;
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