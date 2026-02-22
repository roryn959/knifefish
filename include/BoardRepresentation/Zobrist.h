#pragma once

#include <array>
#include <cstdint>
#include <random>

#include "Config.h"
#include "BoardRepresentation/Types.h"
#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Square.h"

typedef uint64_t Hash;
typedef std::array<Hash, static_cast<size_t>(Square::NUMBER_OF_SQUARES)> PieceHashValues;
typedef std::array<PieceHashValues, Piece::NUM_PIECES> PieceHashValuesList;


class Zobrist {
public:
	Zobrist();

	inline Hash GetHash() const noexcept { return m_hash; } 

	void ApplyHash(Piece piece, Square square);
	void ApplyCastleHash(CastlePermission castlePermission);
	void ApplyEnPassantHash(Square square);
	void ApplyWhiteTurnHash();

private:
	PieceHashValuesList 												m_pieceHashes;
	std::array<Hash, static_cast<size_t>(CastlePermission::COUNT)> 		m_castleHashes;
	std::array<Hash, static_cast<size_t>(Square::NUMBER_OF_SQUARES)> 	m_enPassantHashes;
	Hash																m_whiteTurnHash;

	Hash m_hash;
};
