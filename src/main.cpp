#include <iostream>

#include "Interface/Interface.h"

#include "BoardRepresentation/Board.h"
#include "Engine/MoveGenerator.h"
#include "Engine/Player.h"


int main() {
	Interface interface;

	interface.ListenForConnection();
	interface.ListenForCommands();

	return 0;
}
