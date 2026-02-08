#include <iostream>

#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Board.h"
#include "BoardRepresentation/Square.h"
#include "Engine/Move.h"
#include "Engine/MoveGenerator.h"


int main() {
	Board board;
	MoveGenerator mg(board);

	std::cout << board;

	std::vector<Move> moves = mg.GenerateMoves();

	for (Move m : moves) {
		std::cout << m;
	}

	return 0;
}
