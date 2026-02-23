#include <iostream>

#include "Interface/Interface.h"

#include "BoardRepresentation/Board.h"
#include "Engine/MoveGenerator.h"
#include "Engine/Player.h"


int main() {
	Board board;
	MoveGenerator mg(board);
	Player player(board);

	board.SetUpStartPosition();
	player.GoDepth(7);
	return 0;

	Interface interface;

	interface.ListenForConnection();
	interface.ListenForCommands();

	return 0;
}
