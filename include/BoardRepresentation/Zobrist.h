#pragma once

#include <array>
#include <cstdint>
#include <random>

#include "Config.h"
#include "BoardRepresentation/Types.h"
#include "BoardRepresentation/Pieces.h"
#include "BoardRepresentation/Square.h"

typedef uint64_t Hash;
typedef std::array<Hash, static_cast<size_t>(Square::COUNT)> PieceHashValues;
typedef std::array<PieceHashValues, Piece::NUM_PIECES> PieceHashValuesList;


class Zobrist {
public:
	Zobrist();

	inline Hash GetHash() const noexcept { return m_hash; } 
	inline void ResetHash() noexcept { m_hash = 0ULL; }

	void ApplyPieceHash(Piece piece, Square square);
	void ApplyCastleHash(CastlePermission castlePermission);
	void ApplyEnPassantHash(Square square);
	void ApplyWhiteTurnHash();

private:
	PieceHashValuesList 												m_pieceHashes;

	// Castlehashes size is 8 so it's cleaner and easier to fid the hash for a permission
	std::array<Hash, 8> 												m_castleHashes;

	std::array<Hash, static_cast<size_t>(Square::COUNT)> 				m_enPassantHashes;

	Hash																m_whiteTurnHash;

	Hash m_hash;
};
