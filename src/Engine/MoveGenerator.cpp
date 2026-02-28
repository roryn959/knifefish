#include "Engine/MoveGenerator.h"


MoveGenerator::MoveGenerator(Board& board) :
	m_board{board},
	m_magicBitboardHelper{}
{}

std::vector<Move> MoveGenerator::GenerateLegalMoves() {
	std::vector<Move> pseudoMoves = GeneratePseudoMoves();

#if DEBUG
	Hash beforeHash = m_board.GetHash();
#endif

	std::vector<Move> legalMoves;
	legalMoves.reserve(218);

	if (m_board.IsWhiteTurn())
		legalMoves = FilterOutIllegalWhiteMoves(pseudoMoves);
	else
		legalMoves = FilterOutIllegalBlackMoves(pseudoMoves);

#if DEBUG
	Hash afterHash = m_board.GetHash();
	if (beforeHash != afterHash) {
		std::cerr << "Error: Hashing issue detected during move generation\n";
		std::exit(1);
	}
#endif

	return legalMoves;
}

std::vector<Move> MoveGenerator::FilterOutIllegalWhiteMoves(std::vector<Move>& moves) {
	std::vector<Move> legalMoves;

	for (Move& move : moves) {
		Undo undo = m_board.MakeMove(move);

		Bitboard king = m_board.GetPieceBitboard(Piece::WHITE_KING);
		if (!IsAttackedByBlack(king))
			legalMoves.push_back(move);
		
		m_board.UndoMove(move, undo);
	}

	return legalMoves;
}

std::vector<Move> MoveGenerator::FilterOutIllegalBlackMoves(std::vector<Move>& moves) {
	std::vector<Move> legalMoves;

	for (Move& move : moves) {
		Undo undo = m_board.MakeMove(move);

		Bitboard king = m_board.GetPieceBitboard(Piece::BLACK_KING);
		if (!IsAttackedByWhite(king))
			legalMoves.push_back(move);
		
		m_board.UndoMove(move, undo);
	}

	return legalMoves;
} 

std::vector<Move> MoveGenerator::GeneratePseudoMoves() {
	m_occupiedSquares = m_board.GetAllPieceBitboard();
	m_emptySquares = ~m_occupiedSquares;

	std::vector<Move> moves;
	moves.reserve(218);

	if (m_board.IsWhiteTurn()) {
		PrepareWhiteMoveGeneration();
		GenerateWhitePawnMoves(moves);
		GenerateWhiteCastleMoves(moves);
	}
	else {
		PrepareBlackMoveGeneration();
		GenerateBlackPawnMoves(moves);
		GenerateBlackCastleMoves(moves);
	}

	GenerateKnightMoves(moves);
	GenerateBishopMoves(moves);
	GenerateRookMoves(moves);
	GenerateQueenMoves(moves);
	GenerateKingMoves(moves);
	
	return moves;
}

void MoveGenerator::PrepareWhiteMoveGeneration() {
	m_friendlyPieces = m_board.GetWhitePieceBitboard();
	m_enemyPieces = m_board.GetBlackPieceBitboard();
	m_enemyAttackSet = GetBlackAttackSet();

	m_pawns = m_board.GetPieceBitboard(Piece::WHITE_PAWN);
	m_knights = m_board.GetPieceBitboard(Piece::WHITE_KNIGHT);
	m_bishops = m_board.GetPieceBitboard(Piece::WHITE_BISHOP);
	m_rooks = m_board.GetPieceBitboard(Piece::WHITE_ROOK);
	m_queens = m_board.GetPieceBitboard(Piece::WHITE_QUEEN);
	m_king = m_board.GetPieceBitboard(Piece::WHITE_KING);
}

void MoveGenerator::PrepareBlackMoveGeneration() {
	m_friendlyPieces = m_board.GetBlackPieceBitboard();
	m_enemyPieces = m_board.GetWhitePieceBitboard();
	m_enemyAttackSet = GetWhiteAttackSet();

	m_pawns = m_board.GetPieceBitboard(Piece::BLACK_PAWN);
	m_knights = m_board.GetPieceBitboard(Piece::BLACK_KNIGHT);
	m_bishops = m_board.GetPieceBitboard(Piece::BLACK_BISHOP);
	m_rooks = m_board.GetPieceBitboard(Piece::BLACK_ROOK);
	m_queens = m_board.GetPieceBitboard(Piece::BLACK_QUEEN);
	m_king = m_board.GetPieceBitboard(Piece::BLACK_KING);
}

void MoveGenerator::GenerateWhitePawnMoves(std::vector<Move>& moves) {
	while (m_pawns.Any()) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = pawn.ShiftNorth();
		
		GenerateWhitePawnPushes(moves, pawn, push);
		GenerateWhitePawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateWhitePawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard northMove = push & m_emptySquares;
	
	if (northMove.Empty()) return;

	Bitboard seventhRank = pawn & RANK_7_MASK;
	if (seventhRank.Any()) {
		moves.push_back(Move{pawn, northMove, Piece::WHITE_KNIGHT, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_BISHOP, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_ROOK, false, false, false, false});
		moves.push_back(Move{pawn, northMove, Piece::WHITE_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{pawn, northMove, Piece::EMPTY, false, false, false, false});
	}

	Bitboard secondRank = pawn & RANK_2_MASK;
	if (secondRank.Empty()) return;

	Bitboard northNorthMove = northMove.ShiftNorth() & m_emptySquares;
	if (northNorthMove.Any()) {
		moves.push_back(Move{pawn, northNorthMove, Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateWhitePawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard seventhRank = pawn & RANK_7_MASK;

	Bitboard westCaptureMove = push.ShiftWest() & m_enemyPieces;
	if (westCaptureMove.Any()) {
		if (seventhRank.Any()) {
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, westCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = push.ShiftEast() & m_enemyPieces;
	if (eastCaptureMove.Any()) {
		if (seventhRank.Any()) {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::WHITE_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard westEnPassantMove = push.ShiftWest() & m_board.GetEnPassantSquare();
	if (westEnPassantMove.Any()) {
		moves.push_back(Move{pawn, westEnPassantMove, Piece::EMPTY, false, false, true, false});
	}

	Bitboard eastEnPassantMove = push.ShiftEast() & m_board.GetEnPassantSquare();
	if (eastEnPassantMove.Any()) {
		moves.push_back(Move{pawn, eastEnPassantMove, Piece::EMPTY, false, false, true, false});
	}
}

void MoveGenerator::GenerateBlackPawnMoves(std::vector<Move>& moves) {
	while (m_pawns.Any()) {
		Bitboard pawn = m_pawns.PopLsb();
		Bitboard push = pawn.ShiftSouth();
		
		GenerateBlackPawnPushes(moves, pawn, push);
		GenerateBlackPawnCaptures(moves, pawn, push);
	}
}

void MoveGenerator::GenerateBlackPawnPushes(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard southMove = push & m_emptySquares;
	
	if (southMove.Empty()) return;

	Bitboard secondRank = pawn & RANK_2_MASK;
	if (secondRank.Any()) {
		moves.push_back(Move{pawn, southMove, Piece::BLACK_KNIGHT, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_BISHOP, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_ROOK, false, false, false, false});
		moves.push_back(Move{pawn, southMove, Piece::BLACK_QUEEN, false, false, false, false});
	} else {
		moves.push_back(Move{pawn, southMove, Piece::EMPTY, false, false, false, false});
	}

	Bitboard seventhRank = pawn & RANK_7_MASK;
	if (seventhRank.Empty()) return;

	Bitboard southSouthMove = southMove.ShiftSouth() & m_emptySquares;
	if (southSouthMove.Any()) {
		moves.push_back(Move{pawn, southSouthMove, Piece::EMPTY, false, true, false, false});
	}
}

void MoveGenerator::GenerateBlackPawnCaptures(std::vector<Move>& moves, Bitboard pawn, Bitboard push) {
	Bitboard secondRank = pawn & RANK_2_MASK;

	Bitboard westCaptureMove = push.ShiftWest() & m_enemyPieces;
	if (westCaptureMove.Any()) {
		if (secondRank.Any()) {
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, westCaptureMove, Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, westCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard eastCaptureMove = push.ShiftEast() & m_enemyPieces;
	if (eastCaptureMove.Any()) {
		if (secondRank.Any()) {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_KNIGHT, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_BISHOP, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_ROOK, true, false, false, false});
			moves.push_back(Move{pawn, eastCaptureMove, Piece::BLACK_QUEEN, true, false, false, false});
		} else {
			moves.push_back(Move{pawn, eastCaptureMove, Piece::EMPTY, true, false, false, false});
		}
	}

	Bitboard westEnPassantMove = push.ShiftWest() & m_board.GetEnPassantSquare();
	if (westEnPassantMove.Any()) {
		moves.push_back(Move{pawn, westEnPassantMove, Piece::EMPTY, false, false, true, false});
	}

	Bitboard eastEnPassantMove = push.ShiftEast() & m_board.GetEnPassantSquare();
	if (eastEnPassantMove.Any()) {
		moves.push_back(Move{pawn, eastEnPassantMove, Piece::EMPTY, false, false, true, false});
	}
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) {
	while (m_knights.Any()) {
		Bitboard knight = m_knights.PopLsb();

		GenerateKnightHopsNorth(moves, knight);
		GenerateKnightHopsEast(moves, knight);
		GenerateKnightHopsSouth(moves, knight);
		GenerateKnightHopsWest(moves, knight);
	}
}

void MoveGenerator::GenerateKnightHopsNorth(std::vector<Move>& moves, Bitboard knight) {
	Bitboard north = (knight << 16);

	if (north.Empty()) return;
	
	Bitboard nnw = north.ShiftWest();
	if ((nnw & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, nnw, Piece::EMPTY, true, false, false, false});
	} else if ((nnw & m_emptySquares).Any()) {
		moves.push_back(Move{knight, nnw, Piece::EMPTY, false, false, false, false});
	}

	Bitboard nne = north.ShiftEast();
	if ((nne & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, nne, Piece::EMPTY, true, false, false, false});
	} else if ((nne & m_emptySquares).Any()) {
		moves.push_back(Move{knight, nne, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsEast(std::vector<Move>& moves, Bitboard knight) {
	Bitboard east = (knight & NON_FILE_G_OR_H_MASK) >> 2;

	if (east.Empty()) return;

	Bitboard een = east.ShiftNorth();
	if ((een & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, een, Piece::EMPTY, true, false, false, false});
	} else if ((een & m_emptySquares).Any()) {
		moves.push_back(Move{knight, een, Piece::EMPTY, false, false, false, false});
	}
	
	Bitboard ees = east.ShiftSouth();
	if ((ees & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, ees, Piece::EMPTY, true, false, false, false});
	} else if ((ees & m_emptySquares).Any()) {
		moves.push_back(Move{knight, ees, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsSouth(std::vector<Move>& moves, Bitboard knight) {
	Bitboard south = (knight >> 16);

	if (south.Empty()) return;

	Bitboard ssw = south.ShiftWest();
	if ((ssw & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, ssw, Piece::EMPTY, true, false, false, false});
	} else if ((ssw & m_emptySquares).Any()) {
		moves.push_back(Move{knight, ssw, Piece::EMPTY, false, false, false, false});
	}

	Bitboard sse = south.ShiftEast();
	if ((sse & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, sse, Piece::EMPTY, true, false, false, false});
	} else if ((sse & m_emptySquares).Any()) {
		moves.push_back(Move{knight, sse, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKnightHopsWest(std::vector<Move>& moves, Bitboard knight) {
	Bitboard west = (knight & NON_FILE_A_OR_B_MASK) << 2;

	if (west.Empty()) return;

	Bitboard wwn = west.ShiftNorth();
	if ((wwn & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, wwn, Piece::EMPTY, true, false, false, false});
	} else if ((wwn & m_emptySquares).Any()) {
		moves.push_back(Move{knight, wwn, Piece::EMPTY, false, false, false, false});
	}
	
	Bitboard wws = west.ShiftSouth();
	if ((wws & m_enemyPieces).Any()) {
		moves.push_back(Move{knight, wws, Piece::EMPTY, true, false, false, false});
	} else if ((wws & m_emptySquares).Any()) {
		moves.push_back(Move{knight, wws, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GeneratePieceSlideNorthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftNorthEast();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftNorthEast();
	}
}

void MoveGenerator::GeneratePieceSlideSouthEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftSouthEast();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftSouthEast();
	}
}

void MoveGenerator::GeneratePieceSlideSouthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftSouthWest();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftSouthWest();
	}
}

void MoveGenerator::GeneratePieceSlideNorthWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftNorthWest();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftNorthWest();
	}
}

void MoveGenerator::GeneratePieceSlideNorth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftNorth();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftNorth();
	}
}

void MoveGenerator::GeneratePieceSlideEast(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftEast();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftEast();
	}
}

void MoveGenerator::GeneratePieceSlideSouth(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftSouth();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftSouth();
	}
}

void MoveGenerator::GeneratePieceSlideWest(std::vector<Move>& moves, Bitboard piece) {
	Bitboard shadow = piece.ShiftWest();
	while (shadow.Any()) {
		Bitboard capture = shadow & m_enemyPieces;
		if (capture.Any()) {
			moves.push_back(Move{piece, shadow, Piece::EMPTY, true, false, false, false});
			return;
		}

		Bitboard blocker = shadow & m_friendlyPieces;
		if (blocker.Any()) {
			return;
		}

		moves.push_back(Move{piece, shadow, Piece::EMPTY, false, false, false, false});
		shadow = shadow.ShiftWest();
	}
}

void MoveGenerator::GenerateKingNorthMove(std::vector<Move>& moves) {
	Bitboard north = m_king.ShiftNorth();

	if (north.Empty()) return;

	if ((north & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, north, Piece::EMPTY, true, false, false, false});
	} else if ((north & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, north, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingEastMoves(std::vector<Move>& moves) {
	Bitboard east = m_king.ShiftEast();

	if (east.Empty()) return;

	if ((east & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, east, Piece::EMPTY, true, false, false, false});
	} else if ((east & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, east, Piece::EMPTY, false, false, false, false});
	}

	Bitboard northEast = east.ShiftNorth();
	if ((northEast & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, northEast, Piece::EMPTY, true, false, false, false});
	} else if ((northEast & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, northEast, Piece::EMPTY, false, false, false, false});
	}

	Bitboard southEast = east.ShiftSouth();
	if ((southEast & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, southEast, Piece::EMPTY, true, false, false, false});
	} else if ((southEast & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, southEast, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingSouthMove(std::vector<Move>& moves) {
	Bitboard south = m_king.ShiftSouth();

	if (south.Empty()) return;

	if ((south & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, south, Piece::EMPTY, true, false, false, false});
	} else if ((south & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, south, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateKingWestMoves(std::vector<Move>& moves) {
	Bitboard west = m_king.ShiftWest();

	if (west.Empty()) return;

	if ((west & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, west, Piece::EMPTY, true, false, false, false});
	} else if ((west & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, west, Piece::EMPTY, false, false, false, false});
	}

	Bitboard northWest = west.ShiftNorth();
	if ((northWest & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, northWest, Piece::EMPTY, true, false, false, false});
	} else if ((northWest & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, northWest, Piece::EMPTY, false, false, false, false});
	}

	Bitboard southWest = west.ShiftSouth();
	if ((southWest & m_enemyPieces).Any()) {
		moves.push_back(Move{m_king, southWest, Piece::EMPTY, true, false, false, false});
	} else if ((southWest & m_emptySquares).Any()) {
		moves.push_back(Move{m_king, southWest, Piece::EMPTY, false, false, false, false});
	}
}

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) {
	while (m_bishops.Any()) {
		Bitboard bishop = m_bishops.PopLsb();

		GeneratePieceSlideNorthEast(moves, bishop);
		GeneratePieceSlideSouthEast(moves, bishop);
		GeneratePieceSlideSouthWest(moves, bishop);
		GeneratePieceSlideNorthWest(moves, bishop);
	}
}

void MoveGenerator::GenerateRookMoves(std::vector<Move>& moves) {
	while (m_rooks.Any()) {
		Bitboard rook = m_rooks.PopLsb();

		GeneratePieceSlideNorth(moves, rook);
		GeneratePieceSlideEast(moves, rook);
		GeneratePieceSlideSouth(moves, rook);
		GeneratePieceSlideWest(moves, rook);
	}
}

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) {
	while (m_queens.Any()) {
		Bitboard queen = m_queens.PopLsb();

		GeneratePieceSlideNorth(moves, queen);
		GeneratePieceSlideEast(moves, queen);
		GeneratePieceSlideSouth(moves, queen);
		GeneratePieceSlideWest(moves, queen);
		GeneratePieceSlideNorthEast(moves, queen);
		GeneratePieceSlideSouthEast(moves, queen);
		GeneratePieceSlideSouthWest(moves, queen);
		GeneratePieceSlideNorthWest(moves, queen);
	}
}

void MoveGenerator::GenerateKingMoves(std::vector<Move>& moves) {
	GenerateKingNorthMove(moves);
	GenerateKingEastMoves(moves);
	GenerateKingSouthMove(moves);
	GenerateKingWestMoves(moves);
}

void MoveGenerator::GenerateWhiteCastleMoves(std::vector<Move>& moves) {
	if (m_board.GetCastlePermission(CastlePermission::WHITE_KINGSIDE)) {
		bool isKingsideClear = (m_occupiedSquares & WHITE_KINGSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & WHITE_KINGSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isKingsideClear && isNotThroughCheck) {
			moves.push_back(Move{m_king, G1_MASK, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.GetCastlePermission(CastlePermission::WHITE_QUEENSIDE)) {
		bool isQueensideClear = (m_occupiedSquares & WHITE_QUEENSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & WHITE_QUEENSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isQueensideClear && isNotThroughCheck) {
			moves.push_back(Move{m_king, C1_MASK, Piece::EMPTY, false, false, false, true});
		}
	}
}

void MoveGenerator::GenerateBlackCastleMoves(std::vector<Move>& moves) {



	if (m_board.GetCastlePermission(CastlePermission::BLACK_KINGSIDE)) {
		bool isKingsideClear = (m_occupiedSquares & BLACK_KINGSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & BLACK_KINGSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isKingsideClear && isNotThroughCheck) {
			moves.push_back(Move{m_king, G8_MASK, Piece::EMPTY, false, false, false, true});
		}
	}

	if (m_board.GetCastlePermission(CastlePermission::BLACK_QUEENSIDE)) {
		bool isQueensideClear = (m_occupiedSquares & BLACK_QUEENSIDE_CASTLE_SPACE_MASK).Empty();
		bool isNotThroughCheck = (m_enemyAttackSet & BLACK_QUEENSIDE_CASTLE_CHECKS_MASK).Empty();
		if (isQueensideClear && isNotThroughCheck) {
			moves.push_back(Move{m_king, C8_MASK, Piece::EMPTY, false, false, false, true});
		}
	}
}

bool MoveGenerator::IsAttackedByWhite(Bitboard square) {
	Bitboard whiteAttackSet = GetWhiteAttackSet();
	return (whiteAttackSet & square).Any();
}

bool MoveGenerator::IsAttackedByBlack(Bitboard square) {
	Bitboard blackAttackSet = GetBlackAttackSet();
	return (blackAttackSet & square).Any();
}

Bitboard MoveGenerator::GetWhiteAttackSet() {
	Bitboard allPieces = m_board.GetAllPieceBitboard();
	Bitboard emptySquares = ~allPieces;

	Bitboard pawns = m_board.GetPieceBitboard(Piece::WHITE_PAWN);
	Bitboard knights = m_board.GetPieceBitboard(Piece::WHITE_KNIGHT);
	Bitboard bishops = m_board.GetPieceBitboard(Piece::WHITE_BISHOP);
	Bitboard rooks = m_board.GetPieceBitboard(Piece::WHITE_ROOK);
	Bitboard queens = m_board.GetPieceBitboard(Piece::WHITE_QUEEN);
	Bitboard king = m_board.GetPieceBitboard(Piece::WHITE_KING);

	Bitboard attackSet = 0ULL;

	attackSet |= GetWhitePawnAttackSet(pawns);
	attackSet |= GetKnightAttackSet(knights);
	attackSet |= GetBishopAttackSet(bishops, allPieces);
	attackSet |= GetRookAttackSet(rooks, allPieces);
	attackSet |= GetQueenAttackSet(queens, allPieces);
	attackSet |= GetKingAttackSet(king);

	return attackSet;
}

Bitboard MoveGenerator::GetBlackAttackSet() {
	Bitboard allPieces = m_board.GetAllPieceBitboard();
	Bitboard emptySquares = ~allPieces;

	Bitboard pawns = m_board.GetPieceBitboard(Piece::BLACK_PAWN);
	Bitboard knights = m_board.GetPieceBitboard(Piece::BLACK_KNIGHT);
	Bitboard bishops = m_board.GetPieceBitboard(Piece::BLACK_BISHOP);
	Bitboard rooks = m_board.GetPieceBitboard(Piece::BLACK_ROOK);
	Bitboard queens = m_board.GetPieceBitboard(Piece::BLACK_QUEEN);
	Bitboard king = m_board.GetPieceBitboard(Piece::BLACK_KING);

	Bitboard attackSet = 0ULL;

	attackSet |= GetBlackPawnAttackSet(pawns);
	attackSet |= GetKnightAttackSet(knights);
	attackSet |= GetBishopAttackSet(bishops, allPieces);
	attackSet |= GetRookAttackSet(rooks, allPieces);
	attackSet |= GetQueenAttackSet(queens, allPieces);
	attackSet |= GetKingAttackSet(king);

	return attackSet;
}

Bitboard MoveGenerator::GetWhitePawnAttackSet(Bitboard pawns) {
	Bitboard attackSet = 0ULL;

	attackSet |= pawns.ShiftNorthWest();
	attackSet |= pawns.ShiftNorthEast();

	return attackSet;
}

Bitboard MoveGenerator::GetBlackPawnAttackSet(Bitboard pawns) {
	Bitboard attackSet = 0ULL;

	if (pawns.Empty()) return attackSet;

	attackSet |= pawns.ShiftSouthWest();
	attackSet |= pawns.ShiftSouthEast();

	return attackSet;
}

Bitboard MoveGenerator::GetKnightAttackSet(Bitboard knights) {
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

Bitboard MoveGenerator::GetBishopAttackSet(Bitboard bishops, Bitboard allPieces) {
	Bitboard attackSet{0ULL};

	for (Square bishop : bishops) {
		Bitboard occupancyMask = GetDiagonalOccupancyMask(bishop);

		Bitboard occupancy = occupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetDiagonalAttacks(bishop, occupancy);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetRookAttackSet(Bitboard rooks, Bitboard allPieces) {
	Bitboard attackSet{0ULL};

	for (Square rook : rooks) {
		Bitboard occupancyMask = GetOrthogonalOccupancyMask(rook);

		Bitboard occupancy = occupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetOrthogonalAttacks(rook, occupancy);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetQueenAttackSet(Bitboard queens, Bitboard allPieces) {
	Bitboard attackSet = 0ULL;
	
	if (queens.Empty()) return attackSet;

	for (Square queen : queens) {
		Bitboard orthogonalOccupancyMask = GetOrthogonalOccupancyMask(queen);
		Bitboard orthogonalOccupancy = orthogonalOccupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetOrthogonalAttacks(queen, orthogonalOccupancy);

		Bitboard diagonalOccupancyMask = GetDiagonalOccupancyMask(queen);
		Bitboard diagonalOccupancy = diagonalOccupancyMask & allPieces;

		attackSet |= m_magicBitboardHelper.GetDiagonalAttacks(queen, diagonalOccupancy);
	}

	return attackSet;
}

Bitboard MoveGenerator::GetKingAttackSet(Bitboard king) {
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