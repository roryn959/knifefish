#pragma once

#include <iostream>

#include "BoardRepresentation/Pieces.h"

#include "Engine/Move.h"
#include "Engine/Undo.h"


class Board {
public:
	Board() = default;

	inline bool IsWhiteTurn() const { return m_isWhiteTurn; }

	inline bool IsWhiteKingsideCastlePermitted() 	const { return m_isWhiteKingsideCastlePermitted; }
	inline bool IsWhiteQueensideCastlePermitted() 	const { return m_isWhiteQueensideCastlePermitted; }
	inline bool IsBlackKingsideCastlePermitted() 	const { return m_isBlackKingsideCastlePermitted; }
	inline bool IsBlackQueensideCastlePermitted() 	const { return m_isBlackQueensideCastlePermitted; }

	bool IsAttackedByWhite(Bitboard squares) const;
	bool IsAttackedByBlack(Bitboard squares) const;

	inline Bitboard GetPieceBitboard(Piece p) const { return m_pieceBitboards[p]; }
	Bitboard GetAllPieceBitboard() const;
	Bitboard GetWhitePieceBitboard() const;
	Bitboard GetBlackPieceBitboard() const;

	Piece GetWhitePieceAtSquare(Bitboard bb);
	Piece GetBlackPieceAtSquare(Bitboard bb);

	inline Bitboard GetEnPassantSquare() const { return m_enPassantSquare; }

	Bitboard GetWhiteAttackSet() const;
	Bitboard GetBlackAttackSet() const;

	void Initialise();
	void SetUpStartPosition();

	inline void PickUp(Piece piece, Bitboard bb) { m_pieceBitboards[piece] &= ~bb; }
	inline void PutDown(Piece piece, Bitboard bb) { m_pieceBitboards[piece] |= bb; }

	Undo MakeMove(const Move& move);
	void MakeCastleMove(const Move& move);
	void MakePromotionMove(const Move& move);
	void MakeSimpleMove(const Move& move);

	void UndoMove(const Move& move, const Undo& undo);
	void UndoCastleMove(const Move& move);
	void UndoPromotionMove(const Move& move, const Undo& undo);
	void UndoSimpleMove(const Move& move, const Undo& undo);

	friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:

	Bitboard GetPawnAttackSet(Bitboard emptySquares, Bitboard pawns) const;
	Bitboard GetKnightAttackSet(Bitboard emptySquares, Bitboard knighs) const;
	Bitboard GetBishopAttackSet(Bitboard emptySquares, Bitboard bishops) const;
	Bitboard GetRookAttackSet(Bitboard emptySquares, Bitboard rooks) const;
	Bitboard GetQueenAttackSet(Bitboard emptySquares, Bitboard queens) const;
	Bitboard GetKingAttackSet(Bitboard emptySquares, Bitboard king) const;

	Bitboard m_pieceBitboards[Piece::NUM_PIECES];

	bool m_isWhiteTurn;

	bool m_isWhiteKingsideCastlePermitted;
	bool m_isWhiteQueensideCastlePermitted;
	bool m_isBlackKingsideCastlePermitted;
	bool m_isBlackQueensideCastlePermitted;

	Bitboard m_enPassantSquare;
};