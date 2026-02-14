#include <iostream>
#include <stack>

#include "BoardRepresentation/Bitboard.h"
#include "BoardRepresentation/Board.h"
#include "BoardRepresentation/Square.h"

#include "Engine/Move.h"
#include "Engine/Player.h"
#include "Engine/Undo.h"


int main() {
	std::stack<Move> moves;
	std::stack<Undo> undos;

	Board board;
	Player player(board);

	std::string userInput;
	while (true) {
		std::cout << board;

		Move move = player.GetMove();

		std::cin >> userInput;

		if (userInput == "t") {
			break;
		} else if (userInput == "r") {
			;
		} else if (userInput == "u") {
			if (moves.size() == 0) {
				std::cout << "No moves to undo.\n";
			} else {
				board.UndoMove(moves.top(), undos.top());
				moves.pop();
				undos.pop();
			}
		} else {
			undos.push(board.MakeMove(move)); moves.push(move);
		}
	}

	return 0;
}
