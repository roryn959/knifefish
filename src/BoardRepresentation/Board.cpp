#include "BoardRepresentation/Board.h"

#include <cassert>


void Board::SetUpStartPosition() {
	// There is some logic for incrementally hashing inside the Set methods.
	// However since we are starting from scratch, we don't really want to use them
	// because we may get weird bugs from hashes not being applied properly if a
	// property doesn't change by resetting the position even though it should since the hash is reset.
	// So, we want to just reset the board manually and apply the hashes manually here (and probably when I do FEN).

	m_zobrist.ResetHash();

	m_isWhiteTurn = true;

	#define X(permission) m_castlePermissions[static_cast<size_t>(CastlePermission::permission)] = true;
	CASTLE_PERMISSIONS_LIST
	#undef X

	m_enPassantSquare = 0ULL;

	#define X(piece) m_pieceBitboards[Piece::piece] = GetStartingPositionBitboard<Piece::piece>();
	PIECES_LIST
	#undef X

	// Update hash

	#define X(piece) 										\
	for (Square sq : m_pieceBitboards[Piece::piece]) { 		\
		m_zobrist.ApplyPieceHash(Piece::piece, sq); 		\
	}
	PIECES_LIST
	#undef X

	#define X(permission) m_zobrist.ApplyCastleHash(CastlePermission::permission);
	CASTLE_PERMISSIONS_LIST
	#undef X

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare.ToSquare());

	m_zobrist.ApplyWhiteTurnHash();
}

#if DEBUG
bool Board::CheckBoardOccupancy() const {
	Bitboard combined = 0ULL;

	#define X(piece) 									\
	Bitboard piece = m_pieceBitboards[Piece::piece]; 	\
	if ((combined & piece).Any()) return false; 		\
	combined |= piece;

	PIECES_LIST
	#undef X

	return true;
}

inline void Board::CheckKingCount(const Move& move) const {
	int whiteKingCount = m_pieceBitboards[Piece::WHITE_KING].PopCount();

	if (whiteKingCount != 1) {
		std::cerr << "White king count error (" << whiteKingCount << ")\n";
		std::cerr << *this;
		std::cerr << move;
		std::exit(1);
	}

	int blackKingCount = m_pieceBitboards[Piece::BLACK_KING].PopCount();
	if (blackKingCount != 1) {
		std::cerr << "Black king count error (" << blackKingCount << ")\n";
		std::cerr << *this;
		std::cerr << move;
		std::exit(1);
	}
}
#endif

bool Board::IsAttackedByWhite(Bitboard square) const {
	Bitboard whiteAttackSet = GetWhiteAttackSet();
	return (whiteAttackSet & square).Any();
}

bool Board::IsAttackedByBlack(Bitboard square) const {
	Bitboard blackAttackSet = GetBlackAttackSet();
	return (blackAttackSet & square).Any();
}

Bitboard Board::GetWhiteAttackSet() const {
	Bitboard emptySquares = ~GetAllPieceBitboard();

	Bitboard pawns = m_pieceBitboards[WHITE_PAWN];
	Bitboard knights = m_pieceBitboards[WHITE_KNIGHT];
	Bitboard bishops = m_pieceBitboards[WHITE_BISHOP];
	Bitboard rooks = m_pieceBitboards[WHITE_ROOK];
	Bitboard queens = m_pieceBitboards[WHITE_QUEEN];
	Bitboard king = m_pieceBitboards[WHITE_KING];

	Bitboard attackSet = 0ULL;

	attackSet |= GetWhitePawnAttackSet(pawns);
	attackSet |= GetKnightAttackSet(knights);
	attackSet |= GetBishopAttackSet(bishops, emptySquares);
	attackSet |= GetRookAttackSet(rooks, emptySquares);
	attackSet |= GetQueenAttackSet(queens, emptySquares);
	attackSet |= GetKingAttackSet(king);

	return attackSet;
}

Bitboard Board::GetBlackAttackSet() const {
	Bitboard emptySquares = ~GetAllPieceBitboard();

	Bitboard pawns = m_pieceBitboards[BLACK_PAWN];
	Bitboard knights = m_pieceBitboards[BLACK_KNIGHT];
	Bitboard bishops = m_pieceBitboards[BLACK_BISHOP];
	Bitboard rooks = m_pieceBitboards[BLACK_ROOK];
	Bitboard queens = m_pieceBitboards[BLACK_QUEEN];
	Bitboard king = m_pieceBitboards[BLACK_KING];

	Bitboard attackSet = 0ULL;

	attackSet |= GetBlackPawnAttackSet(pawns);
	attackSet |= GetKnightAttackSet(knights);
	attackSet |= GetBishopAttackSet(bishops, emptySquares);
	attackSet |= GetRookAttackSet(rooks, emptySquares);
	attackSet |= GetQueenAttackSet(queens, emptySquares);
	attackSet |= GetKingAttackSet(king);

	return attackSet;
}

Bitboard Board::GetWhitePawnAttackSet(Bitboard pawns) const {
	Bitboard attackSet = 0ULL;

	if (pawns.Empty()) return attackSet;

	attackSet |= pawns.ShiftNorthWest();
	attackSet |= pawns.ShiftNorthEast();

	return attackSet;
}

Bitboard Board::GetBlackPawnAttackSet(Bitboard pawns) const {
	Bitboard attackSet = 0ULL;

	if (pawns.Empty()) return attackSet;

	attackSet |= pawns.ShiftSouthWest();
	attackSet |= pawns.ShiftSouthEast();

	return attackSet;
}

Bitboard Board::GetKnightAttackSet(Bitboard knights) const {
	Bitboard attackSet = 0ULL;

	if (knights.Empty()) return attackSet;

	Bitboard north = knights.ShiftNorth();
	attackSet |= north.ShiftNorthWest();
	attackSet |= north.ShiftNorthEast();

	Bitboard east = knights.ShiftEast();
	attackSet |= east.ShiftNorthEast();
	attackSet |= east.ShiftSouthEast();

	Bitboard south = knights.ShiftSouth();
	attackSet |= south.ShiftSouthEast();
	attackSet |= south.ShiftSouthWest();

	Bitboard west = knights.ShiftWest();
	attackSet |= west.ShiftNorthWest();
	attackSet |= west.ShiftSouthWest();

	return attackSet;
}

Bitboard Board::GetBishopAttackSet(Bitboard bishops, Bitboard emptySquares) const {
	Bitboard attackSet = 0ULL;

	if (bishops.Empty()) return attackSet;

	Bitboard shadows;

	shadows = bishops;
	shadows = shadows.ShiftNorthEast();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftNorthEast();
	}

	shadows = bishops;
	shadows = shadows.ShiftSouthEast();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftSouthEast();
	}

	shadows = bishops;
	shadows = shadows.ShiftSouthWest();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftSouthWest();
	}

	shadows = bishops;
	shadows = shadows.ShiftNorthWest();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftNorthWest();
	}

	return attackSet;
}

Bitboard Board::GetRookAttackSet(Bitboard rooks, Bitboard emptySquares) const {
	Bitboard attackSet = 0ULL;

	if (rooks.Empty()) return attackSet;

	Bitboard shadows;
	
	shadows = rooks;
	shadows = shadows.ShiftNorth();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftNorth();
	}

	shadows = rooks;
	shadows = shadows.ShiftEast();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftEast();
	}

	shadows = rooks;
	shadows = shadows.ShiftSouth();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftSouth();
	}

	shadows = rooks;
	shadows = shadows.ShiftWest();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftWest();
	}

	return attackSet;
}

Bitboard Board::GetQueenAttackSet(Bitboard queens, Bitboard emptySquares) const {
	Bitboard attackSet = 0ULL;
	
	if (queens.Empty()) return attackSet;

	Bitboard shadows;

	shadows = queens;
	shadows = shadows.ShiftNorthEast();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftNorthEast();
	}

	shadows = queens;
	shadows = shadows.ShiftSouthEast();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftSouthEast();
	}

	shadows = queens;
	shadows = shadows.ShiftSouthWest();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftSouthWest();
	}

	shadows = queens;
	shadows = shadows.ShiftNorthWest();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftNorthWest();
	}

	shadows = queens;
	shadows = shadows.ShiftNorth();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftNorth();
	}

	shadows = queens;
	shadows = shadows.ShiftEast();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftEast();
	}

	shadows = queens;
	shadows = shadows.ShiftSouth();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftSouth();
	}

	shadows = queens;
	shadows = shadows.ShiftWest();
	while (shadows.Any()) {
		attackSet |= shadows;
		shadows &= emptySquares;
		shadows = shadows.ShiftWest();
	}

	return attackSet;
}

Bitboard Board::GetKingAttackSet(Bitboard king) const {
	Bitboard attackSet = 0ULL;

	attackSet |= king.ShiftNorth();
	attackSet |= king.ShiftNorthEast();
	attackSet |= king.ShiftEast();
	attackSet |= king.ShiftSouthEast();
	attackSet |= king.ShiftSouth();
	attackSet |= king.ShiftSouthWest();
	attackSet |= king.ShiftWest();
	attackSet |= king.ShiftNorthWest();

	return attackSet;
}

Bitboard Board::GetAllPieceBitboard() const {
	Bitboard allPieces{0ULL};

	#define X(piece) allPieces |= m_pieceBitboards[Piece::piece];
	PIECES_LIST
	#undef X

	return allPieces;
}

Bitboard Board::GetWhitePieceBitboard() const {
	Bitboard whitePieces{0ULL};

	#define X(piece) whitePieces |= m_pieceBitboards[Piece::piece];
	WHITE_PIECES_LIST;
	#undef X

	return whitePieces;
}

Bitboard Board::GetBlackPieceBitboard() const {
	Bitboard blackPieces{0ULL};

	#define X(piece) blackPieces |= m_pieceBitboards[Piece::piece];
	BLACK_PIECES_LIST;
	#undef X

	return blackPieces;
}

Piece Board::GetWhitePieceAtSquare(Bitboard bb) {
	Piece piece;
	if ((bb & m_pieceBitboards[Piece::WHITE_PAWN]).Any())
		piece = Piece::WHITE_PAWN;
	else if ((bb & m_pieceBitboards[Piece::WHITE_KNIGHT]).Any())
		piece = Piece::WHITE_KNIGHT;
	else if ((bb & m_pieceBitboards[Piece::WHITE_BISHOP]).Any())
		piece = Piece::WHITE_BISHOP;
	else if ((bb & m_pieceBitboards[Piece::WHITE_ROOK]).Any())
		piece = Piece::WHITE_ROOK;
	else if ((bb & m_pieceBitboards[Piece::WHITE_QUEEN]).Any())
		piece = Piece::WHITE_QUEEN;
	else if ((bb & m_pieceBitboards[Piece::WHITE_KING]).Any())
		piece = Piece::WHITE_KING;
	else
		piece = Piece::EMPTY;
	return piece;
}

Piece Board::GetBlackPieceAtSquare(Bitboard bb) {
	Piece piece;
	if ((bb & m_pieceBitboards[Piece::BLACK_PAWN]).Any())
		piece = Piece::BLACK_PAWN;
	else if ((bb & m_pieceBitboards[Piece::BLACK_KNIGHT]).Any())
		piece = Piece::BLACK_KNIGHT;
	else if ((bb & m_pieceBitboards[Piece::BLACK_BISHOP]).Any())
		piece = Piece::BLACK_BISHOP;
	else if ((bb & m_pieceBitboards[Piece::BLACK_ROOK]).Any())
		piece = Piece::BLACK_ROOK;
	else if ((bb & m_pieceBitboards[Piece::BLACK_QUEEN]).Any())
		piece = Piece::BLACK_QUEEN;
	else if ((bb & m_pieceBitboards[Piece::BLACK_KING]).Any())
		piece = Piece::BLACK_KING;
	else
		piece = Piece::EMPTY;
	return piece;
}

void Board::RebuildHash() {
	m_zobrist.ResetHash();

	#define X(piece) 										\
	for (Square sq : GetPieceBitboard(Piece::piece))		\
		m_zobrist.ApplyPieceHash(Piece::piece, sq);

	PIECES_LIST
	#undef X

	#define X(permission)											\
	if (GetCastlePermission(CastlePermission::permission))			\
		m_zobrist.ApplyCastleHash(CastlePermission::permission);

	CASTLE_PERMISSIONS_LIST
	#undef X

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare.ToSquare());

	if (IsWhiteTurn())
		m_zobrist.ApplyWhiteTurnHash();
}

void Board::SetCastlePermission(CastlePermission castlePermission, bool permitted) noexcept {
	if (permitted == GetCastlePermission(castlePermission)) return;

	m_castlePermissions[static_cast<size_t>(castlePermission)] = permitted;
	m_zobrist.ApplyCastleHash(castlePermission);
}

void Board::SetCastlePermissions(std::array<bool, static_cast<size_t>(CastlePermission::COUNT)> castlePermissions) noexcept {
	#define X(permission) SetCastlePermission(CastlePermission::permission, castlePermissions[static_cast<size_t>(CastlePermission::permission)]);
	CASTLE_PERMISSIONS_LIST
	#undef X
}

void Board::SetEnPassantSquare(Bitboard bb) noexcept {
	if (m_enPassantSquare == bb) return;

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare.ToSquare());
	m_enPassantSquare = bb;
	m_zobrist.ApplyEnPassantHash(m_enPassantSquare.ToSquare());
}

void Board::SwitchTurn() noexcept {
	m_isWhiteTurn = !m_isWhiteTurn;
	m_zobrist.ApplyWhiteTurnHash();
}

bool Board::PickUp(Piece piece, Bitboard bb) {
#if DEBUG
	if (piece == Piece::EMPTY) return false;
	if ((m_pieceBitboards[piece] & bb).Empty()) return false;
#endif

	m_pieceBitboards[piece] &= ~bb; 
	m_zobrist.ApplyPieceHash(piece, bb.ToSquare());
	return true;
}

bool Board::PutDown(Piece piece, Bitboard bb) {
#if DEBUG
	if (piece == Piece::EMPTY) return false;
	if ((GetAllPieceBitboard() & bb).Any()) return false;
#endif

	m_pieceBitboards[piece] |= bb;
	m_zobrist.ApplyPieceHash(piece, bb.ToSquare());
	return true;
}

Undo Board::MakeMove(const Move& move) {
	Undo undo {
		Piece::EMPTY,
		m_enPassantSquare,
		false,
		m_castlePermissions
	};

	SetEnPassantSquare(0ULL);

	if (move.m_isCapture) {
		DoCapture(move, undo);
	} else if (move.m_isEnPassant) {
		DoEnPassantCapture(move);
	} else if (move.m_isDoublePawnPush) {
		DoDoublePawnPush(move);
	}

	if (move.m_isCastle) {
		MakeCastleMove(move);
	} else if (move.m_promotionPiece != Piece::EMPTY) {
		MakePromotionMove(move);
	} else {
		MakeNormalMove(move);
	}

	SwitchTurn();

#if DEBUG
	CheckKingCount(move);
#endif

	return undo;
}

void Board::DoCapture(const Move& move, Undo& undo) {
	Piece piece = IsWhiteTurn() ? GetBlackPieceAtSquare(move.m_to) : GetWhitePieceAtSquare(move.m_to);

	SAFE_CALL_WITH_MOVE(PickUp(piece, move.m_to));

	undo.m_capturedPiece = piece;

	// Turn off castling if we just captured something affecting it
	if (piece == Piece::WHITE_ROOK) {
		if (GetCastlePermission(CastlePermission::WHITE_QUEENSIDE) && (move.m_to == A1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
		} else if (GetCastlePermission(CastlePermission::WHITE_KINGSIDE) && (move.m_to == H1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
		}
	} else if (piece == Piece::BLACK_ROOK) {
		if (GetCastlePermission(CastlePermission::BLACK_QUEENSIDE) && (move.m_to == A8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);
		} else if (GetCastlePermission(CastlePermission::BLACK_KINGSIDE) && (move.m_to == H8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
		}
	}
}

void Board::DoEnPassantCapture(const Move& move) {
	if (IsWhiteTurn()) {
		SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_PAWN, move.m_to.ShiftSouth()));
	} else {
		SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_PAWN, move.m_to.ShiftNorth()));
	}
}

void Board::DoDoublePawnPush(const Move& move) {
	// Only bother setting the en passant square if there is a pawn that could capture because
	// of it. This is relevant when you consider three-fold repetition. An en passant square which
	// does not grant any new possible moves is not sufficient to tell two positions apart, meaning
	// we should *not* include it. This is mostly important because our hash helps us do three-fold
	// repetition detection, so we should not be hashing for an irrelevant en passant square.

	if (IsWhiteTurn()) {
		Bitboard blackPawns = GetPieceBitboard(Piece::BLACK_PAWN);
		if ((blackPawns & move.m_to.ShiftWest()).Any() || (blackPawns & move.m_to.ShiftEast()).Any() )
			SetEnPassantSquare(move.m_from.ShiftNorth());
	}
	else {
		Bitboard whitePawns = GetPieceBitboard(Piece::WHITE_PAWN);
		if ((whitePawns & move.m_to.ShiftWest()).Any() || (whitePawns & move.m_to.ShiftEast()).Any() )
			SetEnPassantSquare(move.m_from.ShiftSouth());
	}
}

void Board::MakeCastleMove(const Move& move) {
	if (IsWhiteTurn()) {
		bool isKingside = (move.m_to == G1_MASK);
		if (isKingside) {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_KING, G1_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_ROOK, H1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_ROOK, F1_MASK));
		} else {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_KING, C1_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_ROOK, A1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_ROOK, D1_MASK));
		}
		SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
		SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
	} else {
		bool isKingside = (move.m_to == G8_MASK);
		if (isKingside) {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_KING, G8_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_ROOK, H8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_ROOK, F8_MASK));
		} else {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_KING, C8_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_ROOK, A8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_ROOK, D8_MASK));
		}
		SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
		SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);

	}
}

void Board::MakePromotionMove(const Move& move) {
	Piece pawn = IsWhiteTurn() ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;
	SAFE_CALL_WITH_MOVE(PickUp(pawn, move.m_from));
	SAFE_CALL_WITH_MOVE(PutDown(move.m_promotionPiece, move.m_to));
}

void Board::MakeNormalMove(const Move& move) {
	Piece piece = IsWhiteTurn() ? GetWhitePieceAtSquare(move.m_from) : GetBlackPieceAtSquare(move.m_from);
	SAFE_CALL_WITH_MOVE(PickUp(piece, move.m_from));
	SAFE_CALL_WITH_MOVE(PutDown(piece, move.m_to));

	// Turn off castling if applicable
	if (IsWhiteTurn()) {
		if (GetCastlePermission(CastlePermission::WHITE_KINGSIDE) && (move.m_from == E1_MASK || move.m_from == H1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
		}
		if (GetCastlePermission(CastlePermission::WHITE_QUEENSIDE) && (move.m_from == E1_MASK || move.m_from == A1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
		}
	} else {
		if (GetCastlePermission(CastlePermission::BLACK_KINGSIDE) && (move.m_from == E8_MASK || move.m_from == H8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
		}
		if (GetCastlePermission(CastlePermission::BLACK_QUEENSIDE) && (move.m_from == E8_MASK || move.m_from == A8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);
		}
	}
}

void Board::UndoMove(const Move& move, const Undo& undo) {
	SwitchTurn();

	if (move.m_isCastle) {
		UndoCastleMove(move);
	} else if (move.m_promotionPiece != Piece::EMPTY) {
		UndoPromotionMove(move, undo);
	} else {
		UndoNormalMove(move);
	}

	if (move.m_isCapture) {
		UndoCapture(move, undo);
	} else if (move.m_isEnPassant) {
		UndoEnPassantCapture(move);
	}

	SetEnPassantSquare(undo.m_enPassantSquare);
	SetCastlePermissions(undo.m_castlePermissions);

#if DEBUG
	CheckKingCount(move);
#endif
}

void Board::UndoCapture(const Move& move, const Undo& undo) {
	SAFE_CALL_WITH_MOVE(PutDown(undo.m_capturedPiece, move.m_to));
}

void Board::UndoEnPassantCapture(const Move& move) {
	if (IsWhiteTurn()) {
		SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_PAWN, move.m_to.ShiftSouth()));
	} else {
		SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_PAWN, move.m_to.ShiftNorth()));
	}
}

void Board::UndoCastleMove(const Move& move) {
	if (IsWhiteTurn()) {
		bool isKingside = (move.m_to == G1_MASK);
		if (isKingside) {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_KING, G1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_ROOK, F1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_ROOK, H1_MASK));
		} else {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_KING, C1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::WHITE_ROOK, D1_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::WHITE_ROOK, A1_MASK));
		}
	} else {
		bool isKingside = (move.m_to == G8_MASK);
		if (isKingside) {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_KING, G8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_ROOK, F8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_ROOK, H8_MASK));
		} else {
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_KING, C8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL_WITH_MOVE(PickUp(Piece::BLACK_ROOK, D8_MASK));
			SAFE_CALL_WITH_MOVE(PutDown(Piece::BLACK_ROOK, A8_MASK));
		}
	}
}

void Board::UndoPromotionMove(const Move& move, const Undo& undo) {
	Piece pawn = IsWhiteTurn() ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;

	SAFE_CALL_WITH_MOVE(PickUp(move.m_promotionPiece, move.m_to));
	SAFE_CALL_WITH_MOVE(PutDown(pawn, move.m_from));
}

void Board::UndoNormalMove(const Move& move) {
	Piece piece = IsWhiteTurn() ? GetWhitePieceAtSquare(move.m_to) : GetBlackPieceAtSquare(move.m_to);

	SAFE_CALL_WITH_MOVE(PickUp(piece, move.m_to));
	SAFE_CALL_WITH_MOVE(PutDown(piece, move.m_from));
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
	Piece boardArray[64];

	for (int i = 0; i < 64; ++i) {
		boardArray[i] = Piece::EMPTY;

		Bitboard bitPosition = 1ULL << i;

		#define X(piece) else if ((bitPosition & board.GetPieceBitboard(Piece::piece)).Any()) boardArray[i] = Piece::piece;
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

	Square enPassant = board.m_enPassantSquare.ToSquare();
	if (enPassant != Square::NONE)
		os << enPassant << '\n';

	return os;
}
