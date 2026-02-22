#include "BoardRepresentation/Zobrist.h"

Zobrist::Zobrist() :
	m_hash{0},
	m_pieceHashes{},
	m_castleHashes{},
	m_enPassantHashes{},
	m_whiteTurnHash{}
{
	std::mt19937_64 rng(69420);

	#define X(piece)																\
	for (int i = 0; i < static_cast<size_t>(Square::NUMBER_OF_SQUARES); ++i) {		\
		m_pieceHashes[Piece::piece][i] = rng(); 									\
	}																				\

	PIECES_LIST
	#undef X

	#define X(castle) m_castleHashes[static_cast<size_t>(CastlePermission::castle)] = rng();
	CASTLE_PERMISSIONS_LIST
	#undef X


	for (int i = static_cast<size_t>(Square::h2); i < static_cast<size_t>(Square::a2); ++i) {
		m_enPassantHashes[i] = rng();
	}

	for (int i = static_cast<size_t>(Square::h7); i < static_cast<size_t>(Square::a7); ++i) {
		m_enPassantHashes[i] = rng();
	}

	m_whiteTurnHash = rng();
}

void Zobrist::ApplyHash(Piece piece, Square square) {
#if DEBUG
	if (piece == Piece::EMPTY) {
		std::cerr << "Error - attempting to hash empty piece on square " << SquareToString(square) << '\n';
		std::exit(1);
	}
#endif

	Hash hash = m_pieceHashes[static_cast<size_t>(piece)][static_cast<size_t>(square)];
	m_hash ^= hash;
}

void Zobrist::ApplyCastleHash(CastlePermission castlePermission) {
	Hash hash = m_castleHashes[static_cast<size_t>(castlePermission)];
	m_hash ^= hash;
}

void Zobrist::ApplyEnPassantHash(Square square) {
#if DEBUG
	if (m_enPassantHashes[static_cast<size_t>(square)] == 0) {
		std::cerr << "Error - attempting to hash invalid square " << SquareToString(square) << '\n';
		std::exit(1);
	}
#endif

	Hash hash = m_enPassantHashes[static_cast<size_t>(square)];
	m_hash ^= hash;
}

void Zobrist::ApplyWhiteTurnHash() {
	Hash hash = m_whiteTurnHash;
	m_hash ^= hash;
}