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

	m_enPassantSquare = Square::NONE;

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

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare);

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

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare);

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

void Board::SetEnPassantSquare(Square square) noexcept {
	if (m_enPassantSquare == square) return;

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare);
	m_enPassantSquare = square;
	m_zobrist.ApplyEnPassantHash(m_enPassantSquare);
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

	SetEnPassantSquare(Square::NONE);

	Bitboard from = Bitboard{move.m_from};
	Bitboard to = Bitboard{move.m_to};
	Piece promotionPiece = move.m_promotionPiece;

	if (move.m_isCapture) {
		DoCapture(from, to, undo);
	} else if (move.m_isEnPassant) {
		DoEnPassantCapture(to);
	} else if (move.m_isDoublePawnPush) {
		DoDoublePawnPush(to);
	}

	if (move.m_isCastle) {
		MakeCastleMove(to);
	} else if (move.m_promotionPiece != Piece::EMPTY) {
		MakePromotionMove(from, to, promotionPiece);
	} else {
		MakeQuietMove(from, to);
	}

	SwitchTurn();

#if DEBUG
	CheckKingCount(move);
#endif

	return undo;
}

void Board::DoCapture(Bitboard from, Bitboard to, Undo& undo) {
	Piece piece = IsWhiteTurn() ? GetBlackPieceAtSquare(to) : GetWhitePieceAtSquare(to);

	SAFE_CALL(PickUp(piece, to));

	undo.m_capturedPiece = piece;

	// Turn off castling if we just captured something affecting it
	if (piece == Piece::WHITE_ROOK) {
		if (GetCastlePermission(CastlePermission::WHITE_QUEENSIDE) && (to == A1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
		} else if (GetCastlePermission(CastlePermission::WHITE_KINGSIDE) && (to == H1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
		}
	} else if (piece == Piece::BLACK_ROOK) {
		if (GetCastlePermission(CastlePermission::BLACK_QUEENSIDE) && (to == A8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);
		} else if (GetCastlePermission(CastlePermission::BLACK_KINGSIDE) && (to == H8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
		}
	}
}

void Board::DoEnPassantCapture(Bitboard to) {
	if (IsWhiteTurn()) {
		SAFE_CALL(PickUp(Piece::BLACK_PAWN, to.ShiftSouth()));
	} else {
		SAFE_CALL(PickUp(Piece::WHITE_PAWN, to.ShiftNorth()));
	}
}

void Board::DoDoublePawnPush(Bitboard to) {
	// Only bother setting the en passant square if there is a pawn that could capture because
	// of it. This is relevant when you consider three-fold repetition. An en passant square which
	// does not grant any new possible moves is not sufficient to tell two positions apart, meaning
	// we should *not* include it. This is mostly important because our hash helps us do three-fold
	// repetition detection, so we should not be hashing for an irrelevant en passant square.

	if (IsWhiteTurn()) {
		Bitboard blackPawns = GetPieceBitboard(Piece::BLACK_PAWN);
		if ((blackPawns & to.ShiftWest()).Any() || (blackPawns & to.ShiftEast()).Any() )
			SetEnPassantSquare(to.ShiftSouth());
	}
	else {
		Bitboard whitePawns = GetPieceBitboard(Piece::WHITE_PAWN);
		if ((whitePawns & to.ShiftWest()).Any() || (whitePawns & to.ShiftEast()).Any() )
			SetEnPassantSquare(to.ShiftNorth());
	}
}

void Board::MakeCastleMove(Bitboard to) {
	if (IsWhiteTurn()) {
		bool isKingside = (to == G1_MASK);
		if (isKingside) {
			SAFE_CALL(PickUp(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_KING, G1_MASK));
			SAFE_CALL(PickUp(Piece::WHITE_ROOK, H1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_ROOK, F1_MASK));
		} else {
			SAFE_CALL(PickUp(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_KING, C1_MASK));
			SAFE_CALL(PickUp(Piece::WHITE_ROOK, A1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_ROOK, D1_MASK));
		}
		SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
		SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
	} else {
		bool isKingside = (to == G8_MASK);
		if (isKingside) {
			SAFE_CALL(PickUp(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_KING, G8_MASK));
			SAFE_CALL(PickUp(Piece::BLACK_ROOK, H8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_ROOK, F8_MASK));
		} else {
			SAFE_CALL(PickUp(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_KING, C8_MASK));
			SAFE_CALL(PickUp(Piece::BLACK_ROOK, A8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_ROOK, D8_MASK));
		}
		SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
		SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);

	}
}

void Board::MakePromotionMove(Bitboard from, Bitboard to, Piece promotionPiece) {
	Piece pawn = IsWhiteTurn() ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;
	SAFE_CALL(PickUp(pawn, from));
	SAFE_CALL(PutDown(promotionPiece, to));
}

void Board::MakeQuietMove(Bitboard from, Bitboard to) {
	Piece piece = IsWhiteTurn() ? GetWhitePieceAtSquare(from) : GetBlackPieceAtSquare(from);
	SAFE_CALL(PickUp(piece, from));
	SAFE_CALL(PutDown(piece, to));

	// Turn off castling if applicable
	if (IsWhiteTurn()) {
		if (GetCastlePermission(CastlePermission::WHITE_KINGSIDE) && (from == E1_MASK || from == H1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
		}
		if (GetCastlePermission(CastlePermission::WHITE_QUEENSIDE) && (from == E1_MASK || from == A1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
		}
	} else {
		if (GetCastlePermission(CastlePermission::BLACK_KINGSIDE) && (from == E8_MASK || from == H8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
		}
		if (GetCastlePermission(CastlePermission::BLACK_QUEENSIDE) && (from == E8_MASK || from == A8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);
		}
	}
}

void Board::UndoMove(const Move& move, const Undo& undo) {
	SwitchTurn();

	Bitboard from{move.m_from};
	Bitboard to{move.m_to};
	Piece promotionPiece = move.m_promotionPiece;
	Piece capturedPiece = undo.m_capturedPiece;

	if (move.m_isCastle) {
		UndoCastleMove(to);
	} else if (move.m_promotionPiece != Piece::EMPTY) {
		UndoPromotionMove(from, to, promotionPiece);
	} else {
		UndoNormalMove(from, to);
	}

	if (move.m_isCapture) {
		UndoCapture(to, capturedPiece);
	} else if (move.m_isEnPassant) {
		UndoEnPassantCapture(to);
	}

	SetEnPassantSquare(undo.m_enPassantSquare);
	SetCastlePermissions(undo.m_castlePermissions);

#if DEBUG
	CheckKingCount(move);
#endif
}

void Board::UndoCapture(Bitboard to, Piece capturedPiece) {
	SAFE_CALL(PutDown(capturedPiece, to));
}

void Board::UndoEnPassantCapture(Bitboard to) {
	if (IsWhiteTurn()) {
		SAFE_CALL(PutDown(Piece::BLACK_PAWN, to.ShiftSouth()));
	} else {
		SAFE_CALL(PutDown(Piece::WHITE_PAWN, to.ShiftNorth()));
	}
}

void Board::UndoCastleMove(Bitboard to) {
	if (IsWhiteTurn()) {
		bool isKingside = (to == G1_MASK);
		if (isKingside) {
			SAFE_CALL(PickUp(Piece::WHITE_KING, G1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL(PickUp(Piece::WHITE_ROOK, F1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_ROOK, H1_MASK));
		} else {
			SAFE_CALL(PickUp(Piece::WHITE_KING, C1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_KING, E1_MASK));
			SAFE_CALL(PickUp(Piece::WHITE_ROOK, D1_MASK));
			SAFE_CALL(PutDown(Piece::WHITE_ROOK, A1_MASK));
		}
	} else {
		bool isKingside = (to == G8_MASK);
		if (isKingside) {
			SAFE_CALL(PickUp(Piece::BLACK_KING, G8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL(PickUp(Piece::BLACK_ROOK, F8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_ROOK, H8_MASK));
		} else {
			SAFE_CALL(PickUp(Piece::BLACK_KING, C8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_KING, E8_MASK));
			SAFE_CALL(PickUp(Piece::BLACK_ROOK, D8_MASK));
			SAFE_CALL(PutDown(Piece::BLACK_ROOK, A8_MASK));
		}
	}
}

void Board::UndoPromotionMove(Bitboard from, Bitboard to, Piece promotionPiece) {
	Piece pawn = IsWhiteTurn() ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;

	SAFE_CALL(PickUp(promotionPiece, to));
	SAFE_CALL(PutDown(pawn, from));
}

void Board::UndoNormalMove(Bitboard from, Bitboard to) {
	Piece piece = IsWhiteTurn() ? GetWhitePieceAtSquare(to) : GetBlackPieceAtSquare(to);

	SAFE_CALL(PickUp(piece, to));
	SAFE_CALL(PutDown(piece, from));
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

	Square enPassant = board.m_enPassantSquare;
	if (enPassant != Square::NONE)
		os << enPassant << '\n';

	for (int i = 0; i < 4; i++) {
		if (board.m_castlePermissions[i])
			os << 'X';
		else
			os << 'O';
	}

	os << '\n';

	return os;
}
