#pragma once

#include <array>
#include <iostream>
#include <random>
#include <vector>

#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Square.h"


constexpr Bitboard FILE_A { 0x8080808080808080 };
constexpr Bitboard FILE_B { 0x4040404040404040 };
constexpr Bitboard FILE_C { 0x2020202020202020 };
constexpr Bitboard FILE_D { 0x1010101010101010 };
constexpr Bitboard FILE_E { 0x0808080808080808 };
constexpr Bitboard FILE_F { 0x0404040404040404 };
constexpr Bitboard FILE_G { 0x0202020202020202 };
constexpr Bitboard FILE_H { 0x0101010101010101 };

constexpr Bitboard RANK_1 { 0x00000000000000FF };
constexpr Bitboard RANK_2 { 0x000000000000FF00 };
constexpr Bitboard RANK_3 { 0x0000000000FF0000 };
constexpr Bitboard RANK_4 { 0x00000000FF000000 };
constexpr Bitboard RANK_5 { 0x000000FF00000000 };
constexpr Bitboard RANK_6 { 0x0000FF0000000000 };
constexpr Bitboard RANK_7 { 0x00FF000000000000 };
constexpr Bitboard RANK_8 { 0xFF00000000000000 };

constexpr Bitboard EMPTY_BOARD { 0ULL };
constexpr Bitboard FULL_BOARD { ~EMPTY_BOARD };
constexpr Bitboard INNER_BOARD { FULL_BOARD & ~FILE_A & ~FILE_H & ~RANK_1 & ~RANK_8 };

// Orthogonal stuff

#define MAX_ORTHOGONAL_RELEVANT_BITS 11
#define ORTHOGONAL_SHIFT (64 - MAX_ORTHOGONAL_RELEVANT_BITS)
#define ORTHOGONAL_CONFIGURATIONS (1 << MAX_ORTHOGONAL_RELEVANT_BITS)

constexpr std::array<Bitboard, 8> FILES = { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
constexpr std::array<Bitboard, 8> RANKS = { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };

constexpr size_t GetRank(Square square) { return static_cast<int>(square) / 8; }
constexpr size_t GetFile(Square square) { return 7 - (static_cast<int>(square) % 8); }

constexpr Bitboard GenerateOrthogonalOccupancyMask(Square square) {
	size_t rank = GetRank(square);
	size_t file = GetFile(square);

	Bitboard occupancyMask = RANKS[rank] | FILES[file];

	if (rank != 0) occupancyMask &= ~RANKS[0];

	if (rank != 7) occupancyMask &= ~RANKS[7];

	if (file != 0) occupancyMask &= ~FILES[0];

	if (file != 7) occupancyMask &= ~FILES[7];

	occupancyMask &= ~GetBitmask(square);

	return occupancyMask;
}

constexpr std::array<Bitboard, static_cast<size_t>(Square::COUNT)> ORTHOGONAL_OCCUPANCY_MASKS {
	#define X(square) GenerateOrthogonalOccupancyMask(Square::square),
	SQUARE_LIST
	#undef X
};

constexpr Bitboard GetOrthogonalOccupancyMask(Square square) {
	return ORTHOGONAL_OCCUPANCY_MASKS[static_cast<size_t>(square)];
}

constexpr std::array<uint64_t, static_cast<size_t>(Square::COUNT)> ORTHOGONAL_MAGICS {
	9403516606337777698ULL, 9259436022746316928ULL, 9296555552813350976ULL, 580964455010144336ULL, 72066390281965643ULL, 1585284807049447429ULL, 9369741224036061192ULL, 360288521021689088ULL,
	4612864696301682688ULL, 869339865770166080ULL, 4611729999433568312ULL, 2884608342553010320ULL, 9079767192076293ULL, 36100270108836352ULL, 18054066844602434ULL, 72339176389175750ULL,
	9277468558701364512ULL, 19292048209322000ULL, 9009432656093256ULL, 1157568246904782976ULL, 81073727361777714ULL, 2378226196678443009ULL, 145346710088977204ULL, 35218734579776ULL,
	9229054313098413121ULL, 9315731013656117504ULL, 2454501586836652544ULL, 3382106390790160ULL, 2537811383157264ULL, 613615502379486500ULL, 720611416809181192ULL, 2205465978949ULL,
	18049588271255552ULL, 379437064970602754ULL, 9557764343548870690ULL, 144255934221324354ULL, 432504186362462336ULL, 18309067945017856ULL, 297522632386414616ULL, 2738751798817853696ULL,
	291680094420731904ULL, 73183666012162048ULL, 9226896109061047296ULL, 4899920828060270594ULL, 4904446524287096834ULL, 6917533425696246306ULL, 4630087453636591712ULL, 594615889375076356ULL,
	17729637580864ULL, 45044929809896512ULL, 11565459416096440452ULL, 35461398528200ULL, 1152930303652921476ULL, 432665041477632512ULL, 9223671112615854184ULL, 126101906803393024ULL,
	1297318513404813889ULL, 114486832924690ULL, 585490011047069761ULL, 4755803817911649474ULL, 149533732372997ULL, 586031348456882306ULL, 143520895729956ULL, 13835903047164903714ULL
};

// Diagonal stuff
#define MAX_DIAGONAL_RELEVANT_BITS 9
#define DIAGONAL_SHIFT (64 - MAX_DIAGONAL_RELEVANT_BITS)
#define DIAGONAL_CONFIGURATIONS (1 << MAX_DIAGONAL_RELEVANT_BITS)

constexpr Bitboard GenerateDiagonalOccupancyMask(Square square) {
	Bitboard bb = GetBitmask(square);

	Bitboard occupancyMask{0ULL};

	Bitboard shadow;

	shadow = bb;
	while (shadow.Any()) {
		shadow = shadow.ShiftNorthEast();
		occupancyMask |= shadow;
	}

	shadow = bb;
	while (shadow.Any()) {
		shadow = shadow.ShiftSouthEast();
		occupancyMask |= shadow;
	}

	shadow = bb;
	while (shadow.Any()) {
		shadow = shadow.ShiftSouthWest();
		occupancyMask |= shadow;
	}

	shadow = bb;
	while (shadow.Any()) {
		shadow = shadow.ShiftNorthWest();
		occupancyMask |= shadow;
	}

	occupancyMask &= INNER_BOARD;

	return occupancyMask;
}

constexpr std::array<Bitboard, static_cast<size_t>(Square::COUNT)> DIAGONAL_OCCUPANCY_MASKS {
	18049651735527936, 70506452091904, 275415828992, 1075975168, 38021120, 8657588224, 2216338399232, 567382630219776,
	9024825867763712, 18049651735527424, 70506452221952, 275449643008, 9733406720, 2216342585344, 567382630203392, 1134765260406784,
	4512412933816832, 9024825867633664, 18049651768822272, 70515108615168, 2491752130560, 567383701868544, 1134765256220672, 2269530512441344,
	2256206450263040, 4512412900526080, 9024834391117824, 18051867805491712, 637888545440768, 1135039602493440, 2269529440784384, 4539058881568768,
	1128098963916800, 2256197927833600, 4514594912477184, 9592139778506752, 19184279556981248, 2339762086609920, 4538784537380864, 9077569074761728,
	562958610993152, 1125917221986304, 2814792987328512, 5629586008178688, 11259172008099840, 22518341868716544, 9007336962655232, 18014673925310464,
	2216338399232, 4432676798464, 11064376819712, 22137335185408, 44272556441600, 87995357200384, 35253226045952, 70506452091904,
	567382630219776, 1134765260406784, 2832480465846272, 5667157807464448, 11333774449049600, 22526811443298304, 9024825867763712, 18049651735527936
};

constexpr std::array<uint64_t, static_cast<size_t>(Square::COUNT)> DIAGONAL_MAGICS {
	1770143906043822720ULL, 9587776292978704ULL, 18061283178841088ULL, 36101366942269440ULL, 4615064680225177664ULL, 77793335225381896ULL, 59379000804352ULL, 72625221228603392ULL,
	36154223217353761ULL, 5631769533484576ULL, 288232989706467331ULL, 36310428099280896ULL, 81065757592518912ULL, 149751353401409536ULL, 1128146713251840ULL, 9243779265415094272ULL,
	1128107671069476ULL, 576742297077809156ULL, 4796334378575725576ULL, 149744722006509696ULL, 617133886706843776ULL, 144401061367546178ULL, 47432935920635904ULL, 22540617586313400ULL,
	9223549086851940960ULL, 5621690803981158433ULL, 577032498618377025ULL, 2900322566696636688ULL, 10088344709090066464ULL, 1153502186870673416ULL, 148637206671558673ULL, 71745283626517ULL,
	4616497485608256000ULL, 76569992526660100ULL, 1152943499135418624ULL, 3463270314687332480ULL, 2333991606665609792ULL, 844564619331584ULL, 2305878760522844168ULL, 725695927944768ULL,
	13520147146883072ULL, 13839579831479708161ULL, 4611695244025533440ULL, 2534392564058702ULL, 3458775818208150593ULL, 9232449747267698704ULL, 9641617614963328ULL, 1154048229248155904ULL,
	468951627939258369ULL, 18580648474446096ULL, 4644475092657664ULL, 6917529027926884360ULL, 722376654884ULL, 68239270875138ULL, 288525193846981648ULL, 571750410913840ULL,
	9223374305746763856ULL, 301884120137760832ULL, 9264469657640241161ULL, 148690256237038592ULL, 72060618769204320ULL, 2017612650246307904ULL, 2305948564544700562ULL, 612772158178951172ULL
};

constexpr Bitboard GetDiagonalOccupancyMask(Square square) {
	return DIAGONAL_OCCUPANCY_MASKS[static_cast<size_t>(square)];
}

class MagicBitboardHelper {
public:
	MagicBitboardHelper();

	inline Bitboard GetOrthogonalAttacks(Square square, Bitboard configuration) const { return m_orthogonalAttacks[static_cast<size_t>(square)][GetOrthogonalIndex(square, configuration)]; }
	inline Bitboard GetDiagonalAttacks(Square square, Bitboard configuration) const { return m_diagonalAttacks[static_cast<size_t>(square)][GetDiagonalIndex(square, configuration)]; }

private:
	inline uint64_t GenerateMagic() { return m_rng() & m_rng() & m_rng(); }

	// Orthogonal stuff

	inline size_t GetOrthogonalIndex(Square square, Bitboard occupancy) const {
		size_t magic = ORTHOGONAL_MAGICS[static_cast<size_t>(square)];
		return (occupancy * magic) >> ORTHOGONAL_SHIFT;
	}
	
	Bitboard GenerateOrthogonalMoves(Square square, Bitboard configuration);
	void GenerateOrthogonalConfigurationsAndMoves(Square square, std::vector<Bitboard>& configurations, std::vector<Bitboard>& moves);

	uint64_t SearchForOrthogonalMagic(const std::vector<Bitboard>& configurations, const std::vector<Bitboard>& moves);
	void GenerateOrthogonalMagic(Square square);
	void GenerateOrthogonalMagics();

	void PopulateOrthogonalAttacks(Square square);
	void PopulateOrthogonalAttacks();


	// Diagonal stuff

	inline size_t GetDiagonalIndex(Square square, Bitboard occupancy) const {
		size_t magic = DIAGONAL_MAGICS[static_cast<size_t>(square)];
		return (occupancy * magic) >> DIAGONAL_SHIFT;
	}

	Bitboard GenerateDiagonalAttacks(Square square, Bitboard occupancy);
	void GenerateDiagonalOccupanciesAndAttacks(Square square, std::vector<Bitboard>& occupancies, std::vector<Bitboard>& attackSets);

	uint64_t SearchForDiagonalMagic(const std::vector<Bitboard>& occupancies, const std::vector<Bitboard>& attackSets);
	void GenerateDiagonalMagic(Square square);
	void GenerateDiagonalMagics();

	void PopulateDiagonalAttacks(Square square);
	void PopulateDiagonalAttacks();

	std::mt19937_64 m_rng;

	std::array<std::array<Bitboard, ORTHOGONAL_CONFIGURATIONS>, static_cast<size_t>(Square::COUNT)> m_orthogonalAttacks;
	std::array<std::array<Bitboard, DIAGONAL_CONFIGURATIONS>, static_cast<size_t>(Square::COUNT)> m_diagonalAttacks;

};