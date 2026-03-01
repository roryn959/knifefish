#include "Engine/MagicBitboardHelper.h"

MagicBitboardHelper::MagicBitboardHelper() :
	m_rng{}
{
	// Uncomment these when doing one-off calculations
	//GenerateOrthogonalMagics();
	//GenerateDiagonalMagics();

	//GenerateKnightAttacks();
	//GenerateKingAttacks();

	//GenerateOrthogonalRays();
	//GenerateDiagonalRays();

	// Uncomment these when actually running program!
	PopulateOrthogonalAttacks();
	PopulateDiagonalAttacks();
	PopulateBetweenMasks();
}

void MagicBitboardHelper::PopulateBetweenMasks(Square square) {
	std::array<Bitboard, static_cast<size_t>(Square::COUNT)>& betweenMasks = m_betweenMasks[static_cast<size_t>(square)];

	Bitboard squareBB{square};

	Bitboard orthogonalOccupancyMask = GetOrthogonalOccupancyMask(square);
	Bitboard diagonalOccupancyMask = GetDiagonalOccupancyMask(square);

	for (size_t i = 0; i < static_cast<size_t>(Square::COUNT); ++i) {
		Square otherSquare = static_cast<Square>(i);
		Bitboard otherSquareBB{otherSquare};

		bool orthogonallyAligned = (ORTHOGONAL_RAYS[static_cast<size_t>(square)] & otherSquareBB).Any();
		bool diagonallyAligned = (DIAGONAL_RAYS[static_cast<size_t>(square)] & otherSquareBB).Any();

		if (!orthogonallyAligned && !diagonallyAligned) {
			betweenMasks[i] = Bitboard{0ULL};
			continue;
		}

		if (orthogonallyAligned) {

			Bitboard orthogonalOccupancy = orthogonalOccupancyMask & otherSquareBB;
			Bitboard attackSet = GetOrthogonalAttacks(square, orthogonalOccupancy);

			Bitboard otherOccupancyMask = GetOrthogonalOccupancyMask(otherSquare);
			Bitboard otherOccupancy = otherOccupancyMask & squareBB;
			Bitboard otherAttackSet = GetOrthogonalAttacks(otherSquare, otherOccupancy);

			betweenMasks[i] = attackSet & otherAttackSet;
		} else {

			Bitboard diagonalOccupancy = diagonalOccupancyMask & otherSquareBB;
			Bitboard attackSet = GetDiagonalAttacks(square, diagonalOccupancy);

			Bitboard otherOccupancyMask = GetDiagonalOccupancyMask(otherSquare);
			Bitboard otherOccupancy = otherOccupancyMask & squareBB;
			Bitboard otherAttackSet = GetDiagonalAttacks(otherSquare, otherOccupancy);

			betweenMasks[i] = attackSet & otherAttackSet;
		}
	}
}

void MagicBitboardHelper::PopulateBetweenMasks() {
	#define X(square) PopulateBetweenMasks(Square::square);
	SQUARE_LIST
	#undef X
}

void MagicBitboardHelper::GenerateOrthogonalRays(Square square) {
	Bitboard squareBB{square};

	Bitboard rays{0ULL};

	Bitboard shadow;

	shadow = squareBB;
	do {
		shadow = shadow.ShiftNorth();
		rays |= shadow;
	} while (shadow.Any());

	shadow = squareBB;
	do {
		shadow = shadow.ShiftEast();
		rays |= shadow;
	} while (shadow.Any());

	shadow = squareBB;
	do {
		shadow = shadow.ShiftSouth();
		rays |= shadow;
	} while (shadow.Any());

	shadow = squareBB;
	do {
		shadow = shadow.ShiftWest();
		rays |= shadow;
	} while (shadow.Any());

	std::cerr << rays.GetBoard() << ", ";
}

void MagicBitboardHelper::GenerateOrthogonalRays() {
	#define X(square) GenerateOrthogonalRays(Square::square);
	SQUARE_LIST
	#undef X
}

Bitboard MagicBitboardHelper::GenerateOrthogonalAttacks(Square square, Bitboard occupancy) {
	Bitboard squareBB{square};
	Bitboard moves{0ULL};
	Bitboard shadow;
	
	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftNorth();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftEast();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftSouth();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	shadow = squareBB;
	while (true) {
		shadow = shadow.ShiftWest();

		if (shadow.Empty()) break;

		moves |= shadow;

		if ((occupancy & shadow).Any()) break;
	}

	return moves;
}

void MagicBitboardHelper::GenerateOrthogonalOccupanciesAndAttacks(Square square, std::vector<Bitboard>& occupancies, std::vector<Bitboard>& attackSets) {
	Bitboard occupancyMask = GetOrthogonalOccupancyMask(square);
	Bitboard occupancy{0ULL};

	do {
		occupancies.push_back(occupancy);
		Bitboard attackSet = GenerateOrthogonalAttacks(square, occupancy);

		attackSets.push_back(attackSet);

		occupancy = (occupancy - occupancyMask) & occupancyMask;
	} while (occupancy != 0ULL);
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
	std::vector<Bitboard> occupancies;
	std::vector<Bitboard> attackSets;

	GenerateOrthogonalOccupanciesAndAttacks(square, occupancies, attackSets);

	uint64_t magic = SearchForOrthogonalMagic(occupancies, attackSets);
	std::cerr <<  magic << ", ";
}

void MagicBitboardHelper::GenerateOrthogonalMagics() {
	#define X(square) GenerateOrthogonalMagic(Square::square);
	SQUARE_LIST
	#undef X
}

void MagicBitboardHelper::PopulateOrthogonalAttacks(Square square) {
	std::vector<Bitboard> occupancies;
	std::vector<Bitboard> attackSets;

	GenerateOrthogonalOccupanciesAndAttacks(square, occupancies, attackSets);

	std::array<Bitboard, ORTHOGONAL_CONFIGURATIONS>& orthogonalAttacks = m_orthogonalAttacks[static_cast<size_t>(square)];
	
	for (size_t i = 0; i < occupancies.size(); ++i) {
		Bitboard occupancy = occupancies.at(i);
		Bitboard attackSet = attackSets.at(i);

		orthogonalAttacks[GetOrthogonalIndex(square, occupancy)] = attackSet;
	}
}

void MagicBitboardHelper::PopulateOrthogonalAttacks() {
	#define X(square) PopulateOrthogonalAttacks(Square::square);
	SQUARE_LIST
	#undef X
}

void MagicBitboardHelper::GenerateDiagonalRays(Square square) {
	Bitboard squareBB{square};

	Bitboard rays{0ULL};

	Bitboard shadow;

	shadow = squareBB;
	do {
		shadow = shadow.ShiftNorthEast();
		rays |= shadow;
	} while (shadow.Any());

	shadow = squareBB;
	do {
		shadow = shadow.ShiftSouthEast();
		rays |= shadow;
	} while (shadow.Any());

	shadow = squareBB;
	do {
		shadow = shadow.ShiftSouthWest();
		rays |= shadow;
	} while (shadow.Any());

	shadow = squareBB;
	do {
		shadow = shadow.ShiftNorthWest();
		rays |= shadow;
	} while (shadow.Any());

	std::cerr << rays.GetBoard() << ", ";
}

void MagicBitboardHelper::GenerateDiagonalRays() {
	#define X(square) GenerateDiagonalRays(Square::square);
	SQUARE_LIST
	#undef X
}

Bitboard MagicBitboardHelper::GenerateDiagonalAttacks(Square square, Bitboard occupancy) {
	Bitboard squareBB = Bitboard(square);
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

void MagicBitboardHelper::GenerateKnightAttacks(Square square) {
	Bitboard knightBB = Bitboard(square);
	Bitboard attackSet{0ULL};

	attackSet |= knightBB.ShiftNorth().ShiftNorth().ShiftEast();
	attackSet |= knightBB.ShiftNorth().ShiftNorth().ShiftWest();

	attackSet |= knightBB.ShiftEast().ShiftEast().ShiftNorth();
	attackSet |= knightBB.ShiftEast().ShiftEast().ShiftSouth();

	attackSet |= knightBB.ShiftSouth().ShiftSouth().ShiftEast();
	attackSet |= knightBB.ShiftSouth().ShiftSouth().ShiftWest();

	attackSet |= knightBB.ShiftWest().ShiftWest().ShiftSouth();
	attackSet |= knightBB.ShiftWest().ShiftWest().ShiftNorth();

	std::cerr << attackSet.GetBoard() << ", ";

}

void MagicBitboardHelper::GenerateKnightAttacks() {
	#define X(square) GenerateKnightAttacks(Square::square);
	SQUARE_LIST
	#undef X
}

void MagicBitboardHelper::GenerateKingAttacks(Square square) {
	Bitboard kingBB = Bitboard{square};
	Bitboard attackSet{0ULL};

	attackSet |= kingBB.ShiftNorth();
	attackSet |= kingBB.ShiftEast();
	attackSet |= kingBB.ShiftSouth();
	attackSet |= kingBB.ShiftWest();

	attackSet |= kingBB.ShiftNorthEast();
	attackSet |= kingBB.ShiftSouthEast();
	attackSet |= kingBB.ShiftSouthWest();
	attackSet |= kingBB.ShiftNorthWest();

	std::cerr << attackSet.GetBoard() << ", ";
}

void MagicBitboardHelper::GenerateKingAttacks() {
	#define X(square) GenerateKingAttacks(Square::square);
	SQUARE_LIST
	#undef X
}