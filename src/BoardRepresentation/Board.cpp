#include "BoardRepresentation/Board.h"


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

	attackSet |= GetPawnAttackSet(emptySquares, pawns);
	attackSet |= GetKnightAttackSet(emptySquares, knights);
	attackSet |= GetBishopAttackSet(emptySquares, bishops);
	attackSet |= GetRookAttackSet(emptySquares, rooks);
	attackSet |= GetQueenAttackSet(emptySquares, queens);
	attackSet |= GetKingAttackSet(emptySquares, king);

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

	attackSet |= GetPawnAttackSet(emptySquares, pawns);
	attackSet |= GetKnightAttackSet(emptySquares, knights);
	attackSet |= GetBishopAttackSet(emptySquares, bishops);
	attackSet |= GetRookAttackSet(emptySquares, rooks);
	attackSet |= GetQueenAttackSet(emptySquares, queens);
	attackSet |= GetKingAttackSet(emptySquares, king);

	return attackSet;
}

Bitboard Board::GetPawnAttackSet(Bitboard emptySquares, Bitboard pawns) const {
	Bitboard attackSet = 0ULL;

	if (pawns.Empty()) return attackSet;

	if (m_isWhiteTurn) {
		attackSet |= pawns.ShiftSouthWest();
		attackSet |= pawns.ShiftSouthEast();
	} else {
		attackSet |= pawns.ShiftNorthWest();
		attackSet |= pawns.ShiftNorthEast();
	}

	return attackSet;
}

Bitboard Board::GetKnightAttackSet(Bitboard emptySquares, Bitboard knights) const {
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

Bitboard Board::GetBishopAttackSet(Bitboard emptySquares, Bitboard bishops) const {
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

Bitboard Board::GetRookAttackSet(Bitboard emptySquares, Bitboard rooks) const {
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

Bitboard Board::GetQueenAttackSet(Bitboard emptySquares, Bitboard queens) const {
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

Bitboard Board::GetKingAttackSet(Bitboard emptySquares, Bitboard king) const {
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

void Board::Initialise() {
	// Eventually will do things like set up tables and stuff
	;
}

void Board::SetUpStartPosition() {
	m_isWhiteTurn = true;
	m_isWhiteKingsideCastlePermitted = true;
	m_isWhiteQueensideCastlePermitted = true;
	m_isBlackKingsideCastlePermitted = true;
	m_isBlackQueensideCastlePermitted = true;
	m_enPassantSquare = 0ULL;

	#define X(piece) m_pieceBitboards[Piece::piece] = GetStartingPositionBitboard<Piece::piece>();
	PIECES_LIST
	#undef X
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

Undo Board::MakeMove(const Move& move) {
	Undo undo;

	undo.m_isPromotion = move.m_promotionPiece == Piece::EMPTY;
	undo.m_enPassantSquare = m_enPassantSquare;
	undo.m_isWhiteKingsideCastlePermitted = m_isWhiteKingsideCastlePermitted;
	undo.m_isWhiteQueensideCastlePermitted = m_isWhiteQueensideCastlePermitted;
	undo.m_isBlackKingsideCastlePermitted = m_isBlackKingsideCastlePermitted;
	undo.m_isBlackQueensideCastlePermitted = m_isBlackQueensideCastlePermitted;

	if (move.m_isCastle) {
		MakeCastleMove(move);
	} else if (move.m_promotionPiece != Piece::EMPTY) {
		MakePromotionMove(move);
	} else {
		MakeSimpleMove(move);
	}

	m_enPassantSquare = 0LL;

	if (move.m_isCapture) {
		Piece piece = m_isWhiteTurn ? GetBlackPieceAtSquare(move.m_to) : GetWhitePieceAtSquare(move.m_to);
		PickUp(piece, move.m_to);
		undo.m_capturedPiece = piece;
	} else if (move.m_isEnPassant) {
		m_isWhiteTurn ? PickUp(Piece::BLACK_PAWN, move.m_to.ShiftSouth()) : PickUp(Piece::WHITE_PAWN, move.m_to.ShiftNorth());
	} else if (move.m_isDoublePawnPush) {
		m_enPassantSquare = m_isWhiteTurn ? move.m_from.ShiftNorth() : move.m_from.ShiftSouth();
	}

	// Turn off castling permissions if applicable here!

	m_isWhiteTurn = !m_isWhiteTurn;

	return undo;
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

void Board::MakePromotionMove(const Move& move) {
	Piece pawn = m_isWhiteTurn ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;

	PickUp(pawn, move.m_from);
	PutDown(move.m_promotionPiece, move.m_to);
}

void Board::MakeSimpleMove(const Move& move) {
	Piece piece = m_isWhiteTurn ? GetWhitePieceAtSquare(move.m_from) : GetBlackPieceAtSquare(move.m_from);

	PickUp(piece, move.m_from);
	PutDown(piece, move.m_to);
}

void Board::UndoMove(const Move& move, const Undo& undo) {
	m_enPassantSquare = undo.m_enPassantSquare;
	m_isWhiteKingsideCastlePermitted = undo.m_isWhiteKingsideCastlePermitted;
	m_isWhiteQueensideCastlePermitted = undo.m_isWhiteQueensideCastlePermitted;
	m_isBlackKingsideCastlePermitted = undo.m_isBlackKingsideCastlePermitted;
	m_isBlackQueensideCastlePermitted = undo.m_isBlackQueensideCastlePermitted;

	if (move.m_isCastle) {
		UndoCastleMove(move);
	} else if (move.m_promotionPiece != Piece::EMPTY) {
		UndoPromotionMove(move, undo);
	} else {
		UndoSimpleMove(move, undo);
	}

	if (move.m_isCapture) {
		PutDown(undo.m_capturedPiece, move.m_to);
	} else if (move.m_isEnPassant) {
		m_isWhiteTurn ? PutDown(Piece::WHITE_PAWN, move.m_to.ShiftNorth()) : PutDown(Piece::BLACK_PAWN, move.m_to.ShiftSouth());
	}

	m_isWhiteTurn = !m_isWhiteTurn;
}

void Board::UndoCastleMove(const Move& move) {
	// Remember that is it is currently white turn, that means it was black's
	// turn when they castled, so undo black castle move
	if (m_isWhiteTurn) {
		bool isKingside = (move.m_to & G8_MASK).Any();
		if (isKingside) {
			PickUp(Piece::BLACK_KING, G8_MASK);
			PutDown(Piece::BLACK_KING, E8_MASK);
			PickUp(Piece::BLACK_ROOK, F8_MASK);
			PutDown(Piece::BLACK_ROOK, H8_MASK);
		} else {
			PickUp(Piece::BLACK_KING, C8_MASK);
			PutDown(Piece::BLACK_KING, E8_MASK);
			PickUp(Piece::BLACK_ROOK, D8_MASK);
			PutDown(Piece::BLACK_ROOK, A8_MASK);
		}
	} else {
		bool isKingside = (move.m_to & G1_MASK).Any();
		if (isKingside) {
			PickUp(Piece::WHITE_KING, G1_MASK);
			PutDown(Piece::WHITE_KING, E1_MASK);
			PickUp(Piece::WHITE_ROOK, F1_MASK);
			PutDown(Piece::WHITE_ROOK, H1_MASK);
		} else {
			PickUp(Piece::WHITE_KING, G1_MASK);
			PutDown(Piece::WHITE_KING, E1_MASK);
			PickUp(Piece::WHITE_ROOK, F1_MASK);
			PutDown(Piece::WHITE_ROOK, H1_MASK);
		}
	}
}

void Board::UndoPromotionMove(const Move& move, const Undo& undo) {
	// Again remember that the turn will be flipped
	Piece pawn = m_isWhiteTurn ? Piece::BLACK_PAWN : Piece::WHITE_PAWN;

	PickUp(move.m_promotionPiece, move.m_to);
	PutDown(pawn, move.m_from);
}

void Board::UndoSimpleMove(const Move& move, const Undo& undo) {
	Piece piece = m_isWhiteTurn ? GetBlackPieceAtSquare(move.m_to) : GetWhitePieceAtSquare(move.m_to);

	PickUp(piece, move.m_to);
	PutDown(piece, move.m_from);
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

	return os;
}
