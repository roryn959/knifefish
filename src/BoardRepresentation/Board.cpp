#include "BoardRepresentation/Board.h"

#include <cassert>

Board::Board() :
	m_pieceBitboards{},
	m_boardPieces{},
	m_castlePermissions{},
	m_enPassantSquare{},
	m_isWhiteTurn{},
	m_repetitionStack{},
	m_repetitionStackHead{},
	m_repetitionStackTail{},
	m_phase{},
	m_zobrist{}
{
	SetUpStartPosition();
}

void Board::SetUpStartPosition() {
	m_isWhiteTurn = true;

	#define X(permission) SetCastlePermission(CastlePermission::permission, true);
	CASTLE_PERMISSIONS_LIST
	#undef X

	m_enPassantSquare = Square::NONE;

	#define X(piece) m_pieceBitboards[Piece::piece] = 0ULL;
	PIECES_LIST
	#undef X

	m_boardPieces.fill(Piece::EMPTY);

	Bitboard bb;
	#define X(piece) 												\
	bb = GetStartingPositionBitboard<Piece::piece>();				\
	for (Square sq : bb)											\
		PutDown(Piece::piece, sq);
	
	PIECES_LIST
	#undef X

	m_repetitionStackTail = m_repetitionStackHead = 0;
	m_phase = START_PHASE;
	RebuildHash();
}

void Board::SetUpFenPosition(std::istringstream& tokenStream) {

	int j = 63;
	std::array<Piece, static_cast<size_t>(Square::COUNT)> piecePositions;
	piecePositions.fill(Piece::EMPTY);

	// Parse piece positions

	std::string piecePositionString;
	if (!(tokenStream >> piecePositionString))
		std::exit(1);

	for (char c : piecePositionString) {
		if (c == '/')
			continue;
		
		Piece piece = GetPieceFromChar(c);
		if (piece == Piece::EMPTY) {
			int spaces = c - '0';
			j -= spaces;
			continue;
		}

		piecePositions[j--] = piece;
	}

	// Put all those pieces into their bitboards.

	#define X(piece) m_pieceBitboards[static_cast<size_t>(Piece::piece)] = 0ULL;
	PIECES_LIST
	#undef X

	m_boardPieces.fill(Piece::EMPTY);

	#define X(square) PutDown(piecePositions[static_cast<size_t>(Square::square)], Square::square);
	SQUARE_LIST
	#undef X

	// Who's turn to move?

	std::string turnToMoveString;
	if (!(tokenStream >> turnToMoveString))
		std::exit(1);

	if (turnToMoveString == "w")
		m_isWhiteTurn = true;
	else if (turnToMoveString == "b")
		m_isWhiteTurn = false;
	else
		std::exit(1);

	// Castling rights

	SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
	SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
	SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
	SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);

	std::string castlesString;
	if (!(tokenStream >> castlesString))
		std::exit(1);

	for (char c : castlesString) {
		if (c == 'K')
			SetCastlePermission(CastlePermission::WHITE_KINGSIDE, true);
		else if (c == 'Q')
			SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, true);
		else if (c == 'k')
			SetCastlePermission(CastlePermission::BLACK_KINGSIDE, true);
		else if (c == 'q')
			SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, true);
		else
			std::exit(1);
	}

	// En passant

	std::string enPassantString;
	if (!(tokenStream >> enPassantString))
		std::exit(1);

	m_enPassantSquare = StringToSquare(enPassantString); // !!! Check if this en passant square is actually relevant

	// For now I'm ignoring the 50-move count and the halfmove clock and the fullmove number

	// Calculate phase
	m_phase = END_PHASE;
	Piece piece;
	#define X(square) 												\
																	\
	piece = m_boardPieces[static_cast<size_t>(Square::square)]; 	\
	if (piece != Piece::EMPTY)										\
		ProgressPhase(piece);

	SQUARE_LIST
	#undef X

	m_repetitionStackTail = m_repetitionStackHead = 0;
	RebuildHash();
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

void Board::CheckKingCount(const Move& move) const {
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

bool Board::IsRepeatPosition() const noexcept {
	Hash hash = GetHash();

	for (size_t i = m_repetitionStackTail; i < m_repetitionStackHead; ++i) {
		Hash currentHash = m_repetitionStack[i];

		if (currentHash == hash)
			return true;
	}

	return false;
}

bool Board::CheckQuietDraws() const noexcept {
	size_t numReversibleMoves = m_repetitionStackHead - m_repetitionStackTail;

#if DEBUG
	if (numReversibleMoves > MAX_REVERSIBLE_MOVES) {
		std::cerr << "Error: More than " << MAX_REVERSIBLE_MOVES << " reversible moves made!\n";
		std::exit(1);
	}
#endif

	if (numReversibleMoves == MAX_REVERSIBLE_MOVES)
		return true;

	Hash currentHash = GetHash();

	if (m_repetitionStackHead < 2)
		return false;

	int count = 0;
	for (size_t i = (m_repetitionStackHead - 2); i >= m_repetitionStackTail; i -= 2) {
		if (m_repetitionStack.at(i) == currentHash) {
			if (++count == 2) {
				return true;
			}
		}
		
		if (i < 2)
			return false;
	}

	return false;
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

void Board::RebuildHash() {
	m_zobrist.ResetHash();

	#define X(piece) 										\
	for (Square sq : GetPieceBitboard(Piece::piece))		\
		m_zobrist.ApplyPieceHash(Piece::piece, sq);

	PIECES_LIST
	#undef X

	#define X(permission) if (GetCastlePermission(CastlePermission::permission)) m_zobrist.ApplyCastleHash(CastlePermission::permission);
	CASTLE_PERMISSIONS_LIST
	#undef X

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare);

	if (IsWhiteTurn())
		m_zobrist.ApplyWhiteTurnHash();
}

void Board::SetEnPassantSquare(Square square) noexcept {
	if (m_enPassantSquare == square) return;

	m_zobrist.ApplyEnPassantHash(m_enPassantSquare);
	m_enPassantSquare = square;
	m_zobrist.ApplyEnPassantHash(m_enPassantSquare);
}

bool Board::PickUp(Piece piece, const Location& loc) {
#if DEBUG
	if (piece == Piece::EMPTY) return false;
	if ((m_pieceBitboards[piece] & loc.m_bitboard).Empty()) return false;
	if (m_boardPieces[static_cast<size_t>(loc.m_square)] == Piece::EMPTY) return false;
#endif

	m_pieceBitboards[piece] &= ~loc.m_bitboard;
	m_boardPieces[static_cast<size_t>(loc.m_square)] = Piece::EMPTY;
	m_zobrist.ApplyPieceHash(piece, loc.m_square);

	return true;
}

bool Board::PutDown(Piece piece, const Location& loc) {
#if DEBUG
	if (piece == Piece::EMPTY) return false;
	if ((GetAllPieceBitboard() & loc.m_bitboard).Any()) return false;
	if (m_boardPieces[static_cast<size_t>(loc.m_square)] != Piece::EMPTY) return false;
#endif

	m_pieceBitboards[piece] |= loc.m_bitboard;
	m_boardPieces[static_cast<size_t>(loc.m_square)] = piece;
	m_zobrist.ApplyPieceHash(piece, loc.m_square);
	return true;
}

Undo Board::MakeMove(const Move& move) {
	Undo undo {
		Piece::EMPTY,
		m_enPassantSquare,
		m_castlePermissions,
		m_repetitionStackTail
	};

	Hash hash = GetHash();

	SetEnPassantSquare(Square::NONE);

	Location from { move.m_from, Bitboard{move.m_from} };
	Location to { move.m_to, Bitboard{move.m_to} };

	Piece promotionPiece = move.m_promotionPiece;

	bool isReversible = false;

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
		MakeQuietMove(from, to, isReversible);
	}

	if (!isReversible)
		ResetRepetitionStack();
	
	PushToRepetitionStack(hash);
	SwitchTurn();
	++m_moveCount;

#if DEBUG
	CheckKingCount(move);
#endif

	return undo;
}

void Board::DoCapture(const Location& from, const Location& to, Undo& undo) {
	Piece piece = GetPieceAtSquare(to.m_square);

	SAFE_CALL(PickUp(piece, to));

	undo.m_capturedPiece = piece;

	// Turn off castling if we just captured something affecting it
	if (piece == Piece::WHITE_ROOK) {
		if (GetCastlePermission(CastlePermission::WHITE_QUEENSIDE) && (to.m_bitboard == A1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
		} else if (GetCastlePermission(CastlePermission::WHITE_KINGSIDE) && (to.m_bitboard == H1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
		}
	} else if (piece == Piece::BLACK_ROOK) {
		if (GetCastlePermission(CastlePermission::BLACK_QUEENSIDE) && (to.m_bitboard == A8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);
		} else if (GetCastlePermission(CastlePermission::BLACK_KINGSIDE) && (to.m_bitboard == H8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
		}
	}

	ProgressPhase(piece);
}

void Board::DoEnPassantCapture(const Location& to) {
	Bitboard captureBB = IsWhiteTurn() ? to.m_bitboard.ShiftSouth() : to.m_bitboard.ShiftNorth();
	Location captureLoc { static_cast<Square>(captureBB), captureBB };
	if (IsWhiteTurn()) {
		SAFE_CALL(PickUp(Piece::BLACK_PAWN, captureLoc));
	} else {
		SAFE_CALL(PickUp(Piece::WHITE_PAWN, captureLoc));
	}
}

void Board::DoDoublePawnPush(const Location& to) {
	// Only bother setting the en passant square if there is a pawn that could capture because
	// of it. This is relevant when you consider three-fold repetition. An en passant square which
	// does not grant any new possible moves is not sufficient to tell two positions apart, meaning
	// we should *not* include it. This is mostly important because our hash helps us do three-fold
	// repetition detection, so we should not be hashing for an irrelevant en passant square.

	if (IsWhiteTurn()) {
		Bitboard blackPawns = GetPieceBitboard(Piece::BLACK_PAWN);
		if ((blackPawns & to.m_bitboard.ShiftWest()).Any() || (blackPawns & to.m_bitboard.ShiftEast()).Any() )
			SetEnPassantSquare(to.m_bitboard.ShiftSouth());
	}
	else {
		Bitboard whitePawns = GetPieceBitboard(Piece::WHITE_PAWN);
		if ((whitePawns & to.m_bitboard.ShiftWest()).Any() || (whitePawns & to.m_bitboard.ShiftEast()).Any() )
			SetEnPassantSquare(to.m_bitboard.ShiftNorth());
	}
}

void Board::MakeCastleMove(const Location& to) {
	if (IsWhiteTurn()) {
		bool isKingside = (to.m_bitboard == G1_MASK);
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
		bool isKingside = (to.m_bitboard == G8_MASK);
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

void Board::MakePromotionMove(const Location& from, const Location& to, Piece promotionPiece) {
	Piece pawn = IsWhiteTurn() ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;

	SAFE_CALL(PickUp(pawn, from));
	SAFE_CALL(PutDown(promotionPiece, to));

	RegressPhase(promotionPiece);
}

void Board::MakeQuietMove(const Location& from, const Location& to, bool& isReversible) {
	isReversible = true;

	Piece piece = GetPieceAtSquare(from.m_square);
	SAFE_CALL(PickUp(piece, from));
	SAFE_CALL(PutDown(piece, to));

	// Turn off castling if applicable
	if (IsWhiteTurn()) {
		if (GetCastlePermission(CastlePermission::WHITE_KINGSIDE) && (from.m_bitboard == E1_MASK || from.m_bitboard == H1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_KINGSIDE, false);
			isReversible = false;
		}
		if (GetCastlePermission(CastlePermission::WHITE_QUEENSIDE) && (from.m_bitboard == E1_MASK || from.m_bitboard == A1_MASK)) {
			SetCastlePermission(CastlePermission::WHITE_QUEENSIDE, false);
			isReversible = false;
		}
	} else {
		if (GetCastlePermission(CastlePermission::BLACK_KINGSIDE) && (from.m_bitboard == E8_MASK || from.m_bitboard == H8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_KINGSIDE, false);
			isReversible = false;
		}
		if (GetCastlePermission(CastlePermission::BLACK_QUEENSIDE) && (from.m_bitboard == E8_MASK || from.m_bitboard == A8_MASK)) {
			SetCastlePermission(CastlePermission::BLACK_QUEENSIDE, false);
			isReversible = false;
		}
	}

	if (piece == Piece::WHITE_PAWN || piece == Piece::BLACK_PAWN)
		isReversible = false;
}

Undo Board::MakeNullMove() {
	Undo undo {
		Piece::EMPTY,
		m_enPassantSquare,
		m_castlePermissions,
		m_repetitionStackTail
	};

	SetEnPassantSquare(Square::NONE);
	SwitchTurn();
	++m_moveCount;

	return undo;
}

void Board::UndoMove(const Move& move, const Undo& undo) {
	SwitchTurn();

	Location from { move.m_from, Bitboard{move.m_from} };
	Location to { move.m_to, Bitboard{move.m_to}};

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
	PopRepetitionStack();
	m_repetitionStackTail = undo.m_repetitionStackTail;
	--m_moveCount;

#if DEBUG
	CheckKingCount(move);
#endif
}

void Board::UndoCapture(const Location& to, Piece capturedPiece) {
	SAFE_CALL(PutDown(capturedPiece, to));

	RegressPhase(capturedPiece);
}

void Board::UndoEnPassantCapture(const Location& to) {
	if (IsWhiteTurn()) {
		SAFE_CALL(PutDown(Piece::BLACK_PAWN, to.m_bitboard.ShiftSouth()));
	} else {
		SAFE_CALL(PutDown(Piece::WHITE_PAWN, to.m_bitboard.ShiftNorth()));
	}
}

void Board::UndoCastleMove(const Location& to) {
	if (IsWhiteTurn()) {
		bool isKingside = (to.m_bitboard == G1_MASK);
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
		bool isKingside = (to.m_bitboard == G8_MASK);
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

void Board::UndoPromotionMove(const Location& from, const Location& to, Piece promotionPiece) {
	Piece pawn = IsWhiteTurn() ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;

	SAFE_CALL(PickUp(promotionPiece, to));
	SAFE_CALL(PutDown(pawn, from));

	ProgressPhase(promotionPiece);
}

void Board::UndoNormalMove(const Location& from, const Location& to) {
	Piece piece = GetPieceAtSquare(to.m_square);

	SAFE_CALL(PickUp(piece, to));
	SAFE_CALL(PutDown(piece, from));
}

void Board::UndoNullMove(const Undo& undo) {
	SwitchTurn();
	SetEnPassantSquare(undo.m_enPassantSquare);
	SetCastlePermissions(undo.m_castlePermissions);
	m_repetitionStackTail = undo.m_repetitionStackTail;
	--m_moveCount;
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

	#define X(permission) os << (board.GetCastlePermission(CastlePermission::permission) ? 'X' : 'O');
	CASTLE_PERMISSIONS_LIST
	#undef X

	os << "\n\n";

	return os;
}
