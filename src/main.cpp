#include <iostream>

#include "Interface/Interface.h"


int main() {
	// std::stack<Move> pastMoves;
	// std::stack<Undo> undos;

	// Board board;
	// MoveGenerator mg(board);
	// Player player(board);

	// std::string userInput;
	// while (true) {
	// 	std::cout << board;

	// 	Move move = player.GetMove();

	// 	std::cin >> userInput;

	// 	if (userInput == "t") {
	// 		break;
	// 	} else if (userInput == "r") {
	// 		;
	// 	} else if (userInput == "u") {
	// 		if (pastMoves.size() == 0) {
	// 			std::cout << "No moves to undo.\n";
	// 		} else {
	// 			board.UndoMove(pastMoves.top(), undos.top());
	// 			pastMoves.pop();
	// 			undos.pop();
	// 		}
	// 	} else if (userInput == "k") {
	// 		undos.push(board.MakeMove(move));
	// 		pastMoves.push(move);
	// 	} else {
	// 		std::vector<Move> moves = mg.GenerateMoves();
			
	// 		for (const Move& move : moves) {
	// 			std::string moveString = ToString(GetSquareFromBitmask(move.m_from.GetBoard())) + ToString(GetSquareFromBitmask(move.m_to.GetBoard()));
	// 			if (userInput == moveString) {
	// 				undos.push(board.MakeMove(move));
	// 				pastMoves.push(move);
	// 				break;
	// 			}
	// 		}

	// 		std::cout << "No matching move found.\n";
	// 	}
	// }

	Interface interface;

	interface.ListenForConnection();
	interface.ListenForCommands();

	return 0;
}
