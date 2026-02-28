#include "Engine/MagicBitboardHelper.h"

MagicBitboardHelper::MagicBitboardHelper() :
	m_rng{}
{
	PopulateOrthogonalAttacks();
	PopulateDiagonalAttacks();
}

Bitboard MagicBitboardHelper::GenerateOrthogonalMoves(Square square, Bitboard configuration) {
	Bitboard squareBB = GetBitmask(square);
	Bitboard moves = 0ULL;
	Bitboard shadow;
	
	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftNorth();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((configuration & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftEast();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((configuration & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftSouth();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((configuration & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftWest();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((configuration & shadow).Any()) break;
	}

	return moves;
}

void MagicBitboardHelper::GenerateOrthogonalConfigurationsAndMoves(Square square, std::vector<Bitboard>& configurations, std::vector<Bitboard>& moves) {
	Bitboard occupancyMask = GetOrthogonalOccupancyMask(square);

	Bitboard configuration{0ULL};

	do {
		configurations.push_back(configuration);
		Bitboard moveset = GenerateOrthogonalMoves(square, configuration);

		moves.push_back(moveset);

		configuration = (configuration - occupancyMask) & occupancyMask;
	} while (configuration != 0ULL);
}

uint64_t MagicBitboardHelper::SearchForOrthogonalMagic(const std::vector<Bitboard>& configurations, const std::vector<Bitboard>& moves) {
	uint64_t magic;
	while (true) {
		magic = GenerateMagic();
		bool validMagic = true;

		std::array<bool, ORTHOGONAL_CONFIGURATIONS> used;
		used.fill(false);

		std::array<Bitboard, ORTHOGONAL_CONFIGURATIONS> movesets;

		for (int i = 0; i < configurations.size(); ++i) {
			Bitboard configuration = configurations[i];
			Bitboard moveset = moves[i];

			size_t index = (configuration * magic) >> ORTHOGONAL_SHIFT;

			if (!used.at(index)) {
				used.at(index) = true;
				movesets.at(i) = moveset;
			} else if (movesets.at(index) != moveset) {
				validMagic = false;
				break;
			}
		}

		if (validMagic)
			return magic;
	}
}

void MagicBitboardHelper::GenerateOrthogonalMagic(Square square) {
	std::vector<Bitboard> configurations;
	std::vector<Bitboard> moves;

	GenerateOrthogonalConfigurationsAndMoves(square, configurations, moves);

	uint64_t magic = SearchForOrthogonalMagic(configurations, moves);
	std::cerr <<  magic << ", ";
}

void MagicBitboardHelper::GenerateOrthogonalMagics() {
	#define X(square) GenerateOrthogonalMagic(Square::square);
	SQUARE_LIST
	#undef X
}

void MagicBitboardHelper::PopulateOrthogonalAttacks(Square square) {
	std::vector<Bitboard> configurations;
	std::vector<Bitboard> moves;

	GenerateOrthogonalConfigurationsAndMoves(square, configurations, moves);

	std::array<Bitboard, ORTHOGONAL_CONFIGURATIONS>& orthogonalAttacks = m_orthogonalAttacks[static_cast<size_t>(square)];
	
	for (size_t i = 0; i < configurations.size(); ++i) {
		Bitboard configuration = configurations.at(i);
		Bitboard moveSet = moves.at(i);

		orthogonalAttacks[GetOrthogonalIndex(square, configuration)] = moveSet;
	}
}

void MagicBitboardHelper::PopulateOrthogonalAttacks() {
	#define X(square) PopulateOrthogonalAttacks(Square::square);
	SQUARE_LIST
	#undef X
}

Bitboard MagicBitboardHelper::GenerateDiagonalAttacks(Square square, Bitboard occupancy) {
	Bitboard squareBB = GetBitmask(square);
	Bitboard moves = 0ULL;
	Bitboard shadow;
	
	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftNorthEast();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftSouthEast();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftSouthWest();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftNorthWest();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	return moves;
}

void MagicBitboardHelper::GenerateDiagonalOccupanciesAndAttacks(Square square, std::vector<Bitboard>& occupancies, std::vector<Bitboard>& attackSets) {
	Bitboard occupancyMask = GetDiagonalOccupancyMask(square);

	Bitboard occupancy{0ULL};

	do {
		occupancies.push_back(occupancy);

		Bitboard attackSet = GenerateDiagonalAttacks(square, occupancy);
		attackSets.push_back(attackSet);

		occupancy = (occupancy - occupancyMask) & occupancyMask;
	} while (occupancy != 0ULL);
}

uint64_t MagicBitboardHelper::SearchForDiagonalMagic(const std::vector<Bitboard>& occupancies, const std::vector<Bitboard>& attackSets) {
	uint64_t magic;
	while (true) {
		magic = GenerateMagic();
		bool validMagic = true;

		std::array<bool, DIAGONAL_CONFIGURATIONS> used;
		used.fill(false);

		std::array<Bitboard, DIAGONAL_CONFIGURATIONS> attackSetTable;

		for (int i = 0; i < occupancies.size(); ++i) {
			Bitboard occupancy = occupancies[i];
			Bitboard attackSet = attackSets[i];

			size_t index = (occupancy * magic) >> DIAGONAL_SHIFT;

			if (!used.at(index)) {
				used.at(index) = true;
				attackSetTable.at(i) = attackSet;
			} else if (attackSetTable.at(index) != attackSet) {
				validMagic = false;
				break;
			}
		}

		if (validMagic)
			return magic;
	}
}

void MagicBitboardHelper::GenerateDiagonalMagic(Square square) {
	std::vector<Bitboard> occupancies;
	std::vector<Bitboard> attackSets;

	GenerateDiagonalOccupanciesAndAttacks(square, occupancies, attackSets);

	uint64_t magic = SearchForDiagonalMagic(occupancies, attackSets);
	std::cerr <<  magic << ", ";
}

void MagicBitboardHelper::GenerateDiagonalMagics() {
	#define X(square) GenerateDiagonalMagic(Square::square);
	SQUARE_LIST
	#undef X
}

void MagicBitboardHelper::PopulateDiagonalAttacks(Square square) {
	std::vector<Bitboard> occupancies;
	std::vector<Bitboard> attackSets;

	GenerateDiagonalOccupanciesAndAttacks(square, occupancies, attackSets);

	std::array<Bitboard, DIAGONAL_CONFIGURATIONS>& diagonalAttacks = m_diagonalAttacks[static_cast<size_t>(square)];
	
	for (size_t i = 0; i < occupancies.size(); ++i) {
		Bitboard occupancy = occupancies.at(i);
		Bitboard attackSet = attackSets.at(i);

		diagonalAttacks[GetDiagonalIndex(square, occupancy)] = attackSet;
	}
}

void MagicBitboardHelper::PopulateDiagonalAttacks() {
	#define X(square) PopulateDiagonalAttacks(Square::square);
	SQUARE_LIST
	#undef X
}
