#include <iostream>

#include "Interface/Interface.h"

#include "BoardRepresentation/Bitboard.h"
#include "Engine/MagicBitboardHelper.h"


#define TEST 1

int main() {

	Interface interface;

#if TEST

	interface.ProcessCommand("position startpos moves e2e4 b8c6 b1c3 g8f6 d2d4 e7e5 d4e5 c6e5 c1f4 e5c6 e4e5 c6e5 f4e5 d8e7 d1e2 e8d8 c3d5 f6d5 e1c1 d5b4 c1b1 e7e6 b2b3 e6f5 g2g4 f5g6 e2c4 c7c5 f1g2 b4c2");
	//interface.ProcessCommand("show");
	//interface.ProcessCommand("blackattack");
	//interface.ProcessCommand("pseudomoves");
	//interface.ProcessCommand("moves");

	interface.ProcessCommand("go depth 7");
#else
	interface.ListenForConnection();
	interface.ListenForCommands();
#endif

	return 0;
}
