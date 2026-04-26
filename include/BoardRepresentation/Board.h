#pragma once

#include <array>
#include <iostream>
#include <sstream>

#include "Config.h"
#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Types.h"
#include "BoardRepresentation/Zobrist.h"
#include "Engine/Move.h"
#include "Engine/Undo.h"

#define MAX_REVERSIBLE_MOVES 100

#define START_PHASE 24
#define END_PHASE 0
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

struct Location {
	Square m_square;
	Bitboard m_bitboard;
};

class Board {
public:
	Board();

	void SetUpStartPosition();
	void SetUpFenPosition(std::istringstream& tokenStream);

	Undo MakeMove(const Move& move);
	void UndoMove(const Move& move, const Undo& undo);

	Undo MakeNullMove();
	void UndoNullMove(const Undo& undo);

	inline Bitboard GetPieceBitboard(Piece p) const noexcept { return m_pieceBitboards[p]; }
	inline const Bitboard* const GetPieceBitboards() const noexcept { return m_pieceBitboards.data(); }

	Bitboard GetAllPieceBitboard() const;
	Bitboard GetWhitePieceBitboard() const;
	Bitboard GetBlackPieceBitboard() const;

	Piece GetPieceAtSquare(Square square) const noexcept { return m_boardPieces[static_cast<size_t>(square)]; }

	inline int GetMoveCount() const noexcept { return m_moveCount; }

	inline bool GetCastlePermission(CastlePermission castlePermission) const noexcept { return m_castlePermissions & castlePermission;  }

	inline void SetCastlePermission(CastlePermission castlePermission, bool permitted) noexcept { 
		if (permitted)
			m_castlePermissions |= castlePermission;
		else
			m_castlePermissions &= ~castlePermission;
	 }

	inline void SetCastlePermissions(uint8_t castlePermissions) noexcept { m_castlePermissions = castlePermissions; }

	inline Square GetEnPassantSquare() const noexcept { return m_enPassantSquare; }
	void SetEnPassantSquare(Square square) noexcept;
	inline void SetEnPassantSquare(Bitboard squareBB) noexcept { SetEnPassantSquare(static_cast<Square>(squareBB)); }

	inline bool IsWhiteTurn() const noexcept { return m_isWhiteTurn; }
	inline void SwitchTurn() noexcept { m_isWhiteTurn = !m_isWhiteTurn; m_zobrist.ApplyWhiteTurnHash(); }

	bool CheckQuietDraws() const noexcept;

	inline int GetPhase() const noexcept { return (m_phase > START_PHASE) ? START_PHASE : m_phase; }

	inline Hash GetHash() const noexcept { return m_zobrist.GetHash(); }
	void RebuildHash();

	friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
	void DoCapture(const Location& from, const Location& to, Undo& undo);
	void DoEnPassantCapture(const Location& to);
	void DoDoublePawnPush(const Location& to);
	void MakeCastleMove(const Location& to);
	void MakePromotionMove(const Location& from, const Location& to, Piece promotionPiece);
	void MakeQuietMove(const Location& from, const Location& to, bool& isReversible);

	void UndoCapture(const Location& to, Piece capturedPiece);
	void UndoEnPassantCapture(const Location& to);
	void UndoCastleMove(const Location& to);
	void UndoPromotionMove(const Location& from, const Location& to, Piece promotionPiece);
	void UndoNormalMove(const Location& from, const Location& to);

	bool PickUp(Piece piece, const Location& loc);
	inline bool PickUp(Piece piece, Bitboard bb) { return PickUp(piece, Location{ static_cast<Square>(bb), bb }); }
	inline bool PickUp(Piece piece, Square square) { return PickUp(piece, Location{ square, Bitboard{square} }); }

	bool PutDown(Piece piece, const Location& loc);
	inline bool PutDown(Piece piece, Bitboard bb) { return PutDown(piece, Location{ static_cast<Square>(bb), bb }); }
	inline bool PutDown(Piece piece, Square square) { return PutDown(piece, Location{ square, Bitboard{square} }); }

	inline void ResetRepetitionStack() noexcept { m_repetitionStackTail = m_repetitionStackHead; }
	inline void PushToRepetitionStack(Hash hash) noexcept { m_repetitionStack.at(m_repetitionStackHead++) = hash; }
	inline void PopRepetitionStack() noexcept { --m_repetitionStackHead; }

	inline void RegressPhase(Piece piece) noexcept { m_phase += PIECE_PHASE_VALUES[static_cast<size_t>(piece)]; }
	inline void ProgressPhase(Piece piece) noexcept { m_phase -= PIECE_PHASE_VALUES[static_cast<size_t>(piece)]; }

#if DEBUG
	bool CheckBoardOccupancy() const;
	void CheckKingCount(const Move& move) const;
#endif

	std::array<Bitboard, Piece::NUM_PIECES> 				m_pieceBitboards;
	std::array<Piece, static_cast<size_t>(Square::COUNT)> 	m_boardPieces;

	uint8_t 												m_castlePermissions;

	Square 													m_enPassantSquare;
	bool 													m_isWhiteTurn;

	int 													m_moveCount;

	std::array<Hash, 2048>									m_repetitionStack;
	size_t													m_repetitionStackHead;
	size_t 													m_repetitionStackTail;

	int 													m_phase;

	Zobrist 												m_zobrist;
};