#pragma once

#include <array>
#include <iostream>

#include "Config.h"
#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Types.h"
#include "BoardRepresentation/Zobrist.h"
#include "Engine/Move.h"
#include "Engine/Undo.h"


// Macro to do a safe call during move stuff.
#if DEBUG
#define SAFE_CALL_WITH_MOVE(expr)													\
do {																				\
	if (!(expr)) {																	\
		std::cerr << "Error while executing " << #expr << '\n';						\
		std::cerr << *this;															\
		std::cerr << move;															\
		std::exit(1);																\
	} 																				\
	if (!CheckBoardOccupancy()) {													\
		std::cerr << "Board occupancy failure after executing " << #expr << '\n';	\
		std::cerr << *this;															\
		std::cerr << move;															\
		std::exit(1);																\
		}																			\
} while (false)
#else
#define SAFE_CALL_WITH_MOVE(expr) (void) expr
#endif

typedef std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> CastlePermissionsList;


class Board {
public:
	Board() = default;

	void SetUpStartPosition();

	bool IsAttackedByWhite(Bitboard squares) const;
	bool IsAttackedByBlack(Bitboard squares) const;

	inline Bitboard GetPieceBitboard(Piece p) const noexcept { return m_pieceBitboards[p]; }
	Bitboard GetAllPieceBitboard() const;
	Bitboard GetWhitePieceBitboard() const;
	Bitboard GetBlackPieceBitboard() const;

	Piece GetWhitePieceAtSquare(Bitboard bb);
	Piece GetBlackPieceAtSquare(Bitboard bb);

	Bitboard GetWhiteAttackSet() const;
	Bitboard GetBlackAttackSet() const;

	Hash GetHash() const noexcept { return m_zobrist.GetHash(); }
	void RebuildHash();

	inline bool GetCastlePermission(CastlePermission castlePermission) const noexcept { return m_castlePermissions[static_cast<size_t>(castlePermission)]; }
	void SetCastlePermission(CastlePermission castlePermission, bool permitted) noexcept;
	void SetCastlePermissions(std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> castlePermissions) noexcept;

	inline Bitboard GetEnPassantSquare() const noexcept { return m_enPassantSquare; }
	void SetEnPassantSquare(Bitboard bb) noexcept;

	inline bool IsWhiteTurn() const noexcept { return m_isWhiteTurn; }
	void SwitchTurn() noexcept;

	Undo MakeMove(const Move& move);
	void UndoMove(const Move& move, const Undo& undo);

	friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
	Bitboard GetWhitePawnAttackSet(Bitboard pawns) const;
	Bitboard GetBlackPawnAttackSet(Bitboard pawns) const;
	Bitboard GetKnightAttackSet(Bitboard knights) const;
	Bitboard GetBishopAttackSet(Bitboard bishops, Bitboard emptySquares) const;
	Bitboard GetRookAttackSet(Bitboard rooks, Bitboard emptySquares) const;
	Bitboard GetQueenAttackSet(Bitboard queens, Bitboard emptySquares) const;
	Bitboard GetKingAttackSet(Bitboard king) const;

	void DoCapture(const Move& move, Undo& undo);
	void DoEnPassantCapture(const Move& move);
	void DoDoublePawnPush(const Move& move);
	void MakeCastleMove(const Move& move);
	void MakePromotionMove(const Move& move);
	void MakeNormalMove(const Move& move);

	void UndoCapture(const Move& move, const Undo& undo);
	void UndoEnPassantCapture(const Move& move);
	void UndoCastleMove(const Move& move);
	void UndoPromotionMove(const Move& move, const Undo& undo);
	void UndoNormalMove(const Move& move);

	bool PickUp(Piece piece, Bitboard bb);
	bool PutDown(Piece piece, Bitboard bb);

#if DEBUG
	bool CheckBoardOccupancy() const;
	void CheckKingCount(const Move& move) const;
#endif

	Bitboard m_pieceBitboards[Piece::NUM_PIECES];
	std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> m_castlePermissions;
	Bitboard m_enPassantSquare;
	bool m_isWhiteTurn;

	Zobrist m_zobrist;
};