#include <iostream>

#include "Interface/Interface.h"

#include "BoardRepresentation/Bitboard.h"
#include "Engine/MagicBitboardHelper.h"


#define TEST 0

int main() {
	Interface interface;

#if TEST
	interface.ProcessCommand("position startpos moves e2e4 b8c6 b1c3 g8f6 d2d4 e7e5 d4e5 c6e5 c1f4 e5c6 e4e5 c6e5 f4e5 d8e7 d1e2 e8d8 c3d5 f6d5 e1c1 d5b4 c1b1 e7e6 b2b3 e6f5 g2g4 f5g6 e2c4 c7c5 f1g2 b4c2");
	//interface.ProcessCommand("position startpos moves h2h3 h7h6 g2g3 h6h5 f2f3 h5h4 e2e3 h4g3 g1e2 g3g2 f1g2 g7g6 d2d3 g6g5 c2c3 f7f6 b2b3 f6f5 a2a3 e7e6 f3f4 g5f4 e3f4 d7d6 d3d4 d6d5 c3c4 d5c4 b3c4 c7c6 a3a4 c6c5 d4c5 f8c5 a4a5 a7a6 h3h4 g8h6 h4h5 h6g4 b1d2 c5f2 e1f1 g4e3 f1f2 e3d1 h1d1 h8h5 d2e4 d8d1 e4f6 e8f7 f6h5 f7g6 h5g3 d1c2 c1e3 b8d7 a1c1 c2a2 e3b6 d7b6 a5b6 e6e5 f4e5 a2b2 c4c5 b2e5 g3h1 f5f4 g2f3 e5e3 f2g2 c8h3 g2h2 a8h8 f3h5 h8h5 e2f4 e3f4 h1g3 a6a5 h2g1 h5g5 c5c6 a5a4 c6b7 g5g3 g1h1 h3g2 h1g1 a4a3 b7b8n a3a2 b6b7");
	//interface.ProcessCommand("position startpos");
	interface.ProcessCommand("show");
	//interface.ProcessCommand("blackattack");
	//interface.ProcessCommand("moves");
	//interface.ProcessCommand("captures");

	//interface.ProcessCommand("go wtime 11702 btime 12072 winc 100 binc 100");
	interface.ProcessCommand("go depth 9");
	//interface.ProcessCommand("perft 6");

#else
	interface.ListenForConnection();
	interface.ListenForCommands();
#endif

	return 0;
}