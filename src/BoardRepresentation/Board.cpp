#include "BoardRepresentation/Board.h"


Board::Board() :
	m_isWhiteTurn{true},
	m_isWhiteKingsideCastlePermitted{true},
	m_isWhiteQueensideCastlePermitted{true},
	m_isBlackKingsideCastlePermitted{true},
	m_isBlackQueensideCastlePermitted{true}
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

Piece Board::GetWhitePieceAtSquare(Bitboard bb) {
	Piece piece;
	if (!(bb & m_pieceBitboards[Piece::WHITE_PAWN]).IsEmpty())
		piece = Piece::WHITE_PAWN;
	else if (!(bb & m_pieceBitboards[Piece::WHITE_KNIGHT]).IsEmpty())
		piece = Piece::WHITE_KNIGHT;
	else if (!(bb & m_pieceBitboards[Piece::WHITE_BISHOP]).IsEmpty())
		piece = Piece::WHITE_BISHOP;
	else if (!(bb & m_pieceBitboards[Piece::WHITE_ROOK]).IsEmpty())
		piece = Piece::WHITE_ROOK;
	else if (!(bb & m_pieceBitboards[Piece::WHITE_QUEEN]).IsEmpty())
		piece = Piece::WHITE_QUEEN;
	else if (!(bb & m_pieceBitboards[Piece::WHITE_KING]).IsEmpty())
		piece = Piece::WHITE_KING;
	else
		piece = Piece::EMPTY;
	return piece;
}

Piece Board::GetBlackPieceAtSquare(Bitboard bb) {
	Piece piece;
	if (!(bb & m_pieceBitboards[Piece::BLACK_PAWN]).IsEmpty())
		piece = Piece::BLACK_PAWN;
	else if (!(bb & m_pieceBitboards[Piece::BLACK_KNIGHT]).IsEmpty())
		piece = Piece::BLACK_KNIGHT;
	else if (!(bb & m_pieceBitboards[Piece::BLACK_BISHOP]).IsEmpty())
		piece = Piece::BLACK_BISHOP;
	else if (!(bb & m_pieceBitboards[Piece::BLACK_ROOK]).IsEmpty())
		piece = Piece::BLACK_ROOK;
	else if (!(bb & m_pieceBitboards[Piece::BLACK_QUEEN]).IsEmpty())
		piece = Piece::BLACK_QUEEN;
	else if (!(bb & m_pieceBitboards[Piece::BLACK_KING]).IsEmpty())
		piece = Piece::BLACK_KING;
	else
		piece = Piece::EMPTY;
	return piece;
}

void Board::MakeMove(const Move& move) {
	if (move.m_isCastle) {
		MakeCastleMove(move);
	}
}

void Board::MakeSimpleMove(const Move& move) {
	Piece piece;
	if (m_isWhiteTurn) 
		piece = GetWhitePieceAtSquare(move.m_from);
	else
		piece = GetBlackPieceAtSquare(move.m_from);

	PickUp(piece, move.m_from);
	PutDown(piece, move.m_to);
}

void Board::MakeCastleMove(const Move& move) {
	if (m_isWhiteTurn) {
		bool isKingside = (move.m_to == G1_MASK);
		if (isKingside) {
			m_pieceBitboards[Piece::WHITE_KING] = G1_MASK;
			PickUp(Piece::WHITE_ROOK, H1_MASK);
			PutDown(Piece::WHITE_ROOK, F1_MASK);
		} else {
			m_pieceBitboards[Piece::WHITE_KING] = C1_MASK;
			PickUp(Piece::WHITE_ROOK, A1_MASK);
			PutDown(Piece::WHITE_ROOK, D1_MASK);
		}
	} else {
		bool isKingside = (move.m_to == G8_MASK);
		if (isKingside) {
			m_pieceBitboards[Piece::BLACK_KING] = G8_MASK;
			PickUp(Piece::BLACK_ROOK, H8_MASK);
			PutDown(Piece::BLACK_ROOK, F8_MASK);
		} else {
			m_pieceBitboards[Piece::BLACK_KING] = C8_MASK;
			PickUp(Piece::BLACK_ROOK, A8_MASK);
			PutDown(Piece::BLACK_ROOK, D8_MASK);
		}
	}
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
