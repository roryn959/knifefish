#pragma once

#include <iostream>

#include "Engine/Move.h"
#include "Pieces.h"


class Board {
public:
	Board();

	inline bool IsWhiteTurn() const { return m_isWhiteTurn; }

	inline bool IsWhiteKingsideCastlePermitted() 	const { return m_isWhiteKingsideCastlePermitted; }
	inline bool IsWhiteQueensideCastlePermitted() 	const { return m_isWhiteQueensideCastlePermitted; }
	inline bool IsBlackKingsideCastlePermitted() 	const { return m_isBlackKingsideCastlePermitted; }
	inline bool IsBlackQueensideCastlePermitted() 	const { return m_isBlackQueensideCastlePermitted; }

	inline Bitboard GetPieceBitboard(Piece p) const { return m_pieceBitboards[p]; }
	Bitboard GetAllPieceBitboard() const;
	Bitboard GetWhitePieceBitboard() const;
	Bitboard GetBlackPieceBitboard() const;

	Piece GetWhitePieceAtSquare(Bitboard bb);
	Piece GetBlackPieceAtSquare(Bitboard bb);

	inline void PickUp(Piece piece, Bitboard bb) { m_pieceBitboards[piece] &= ~bb; }
	inline void PutDown(Piece piece, Bitboard bb) { m_pieceBitboards[piece] |= bb; }

	void MakeMove(const Move& move);

	void MakeSimpleMove(const Move& move);
	void MakeCastleMove(const Move& move);

	friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
	Bitboard m_pieceBitboards[Piece::NUM_PIECES];

	bool	m_isWhiteTurn;

	bool	m_isWhiteKingsideCastlePermitted;
	bool	m_isWhiteQueensideCastlePermitted;
	bool	m_isBlackKingsideCastlePermitted;
	bool	m_isBlackQueensideCastlePermitted;
};