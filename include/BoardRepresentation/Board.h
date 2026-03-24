#pragma once

#include <array>
#include <iostream>

#include "Config.h"
#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Types.h"
#include "BoardRepresentation/Zobrist.h"
#include "Engine/Move.h"
#include "Engine/Undo.h"

#define START_PHASE 24
constexpr std::array<int, static_cast<size_t>(Piece::NUM_PIECES)> PIECE_PHASE_VALUES { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };

// Macro to do a safe call during move stuff.
#if DEBUG
#define SAFE_CALL(expr)																\
do {																				\
	if (!(expr)) {																	\
		std::cerr << "Error while executing " << #expr << '\n';						\
		std::cerr << *this;															\
		assert(false);																\
	} 																				\
	if (!CheckBoardOccupancy()) {													\
		std::cerr << "Board occupancy failure after executing " << #expr << '\n';	\
		std::cerr << *this;															\
		std::abort();																\
		}																			\
} while (false)
#else
#define SAFE_CALL(expr) (void) expr
#endif

typedef std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> CastlePermissionsList;

struct Location {
	Square m_square;
	Bitboard m_bitboard;
};

class Board {
public:
	Board() = default;

	void SetUpStartPosition();

	inline Bitboard GetPieceBitboard(Piece p) const noexcept { return m_pieceBitboards[p]; }
	inline const Bitboard* const GetPieceBitboards() const noexcept { return m_pieceBitboards.data(); }

	Bitboard GetAllPieceBitboard() const;
	Bitboard GetWhitePieceBitboard() const;
	Bitboard GetBlackPieceBitboard() const;

	Piece GetPieceAtSquare(Square square) const noexcept { return m_boardPieces[static_cast<size_t>(square)]; }

	inline void RegressPhase(Piece piece) noexcept { m_phase += PIECE_PHASE_VALUES[static_cast<size_t>(piece)]; }
	inline void ProgressPhase(Piece piece) noexcept { m_phase -= PIECE_PHASE_VALUES[static_cast<size_t>(piece)]; }

	inline int GetPhase() const noexcept { return (m_phase > START_PHASE) ? START_PHASE : m_phase; }

	Hash GetHash() const noexcept { return m_zobrist.GetHash(); }
	void RebuildHash();

	inline bool GetCastlePermission(CastlePermission castlePermission) const noexcept { return m_castlePermissions[static_cast<size_t>(castlePermission)]; }
	void SetCastlePermission(CastlePermission castlePermission, bool permitted) noexcept;
	void SetCastlePermissions(std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> castlePermissions) noexcept;

	inline Square GetEnPassantSquare() const noexcept { return m_enPassantSquare; }
	void SetEnPassantSquare(Square square) noexcept;
	inline void SetEnPassantSquare(Bitboard squareBB) noexcept { SetEnPassantSquare(static_cast<Square>(squareBB)); }

	inline bool IsWhiteTurn() const noexcept { return m_isWhiteTurn; }
	inline void SwitchTurn() noexcept { m_isWhiteTurn = !m_isWhiteTurn; m_zobrist.ApplyWhiteTurnHash(); }

	Undo MakeMove(const Move& move);
	void UndoMove(const Move& move, const Undo& undo);

	friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
	void DoCapture(const Location& from, const Location& to, Undo& undo);
	void DoEnPassantCapture(const Location& to);
	void DoDoublePawnPush(const Location& to);
	void MakeCastleMove(const Location& to);
	void MakePromotionMove(const Location& from, const Location& to, Piece promotionPiece);
	void MakeQuietMove(const Location& from, const Location& to);

	void UndoCapture(const Location& to, Piece capturedPiece);
	void UndoEnPassantCapture(const Location& to);
	void UndoCastleMove(const Location& to);
	void UndoPromotionMove(const Location& from, const Location& to, Piece promotionPiece);
	void UndoNormalMove(const Location& from, const Location& to);

	bool PickUp(Piece piece, const Location& loc);
	inline bool PickUp(Piece piece, Bitboard bb) { return PickUp(piece, Location{ static_cast<Square>(bb), bb }); }

	bool PutDown(Piece piece, const Location& loc);
	inline bool PutDown(Piece piece, Bitboard bb) { return PutDown(piece, Location{ static_cast<Square>(bb), bb }); }

#if DEBUG
	bool CheckBoardOccupancy() const;
	void CheckKingCount(const Move& move) const;
#endif

	std::array<Bitboard, Piece::NUM_PIECES> m_pieceBitboards;
	std::array<Piece, static_cast<size_t>(Square::COUNT)> m_boardPieces;

	std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> m_castlePermissions;
	Square m_enPassantSquare;
	bool m_isWhiteTurn;

	int m_phase;

	Zobrist m_zobrist;
};