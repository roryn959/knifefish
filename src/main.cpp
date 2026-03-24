#include <iostream>

#include "Interface/Interface.h"

#include "BoardRepresentation/Bitboard.h"
#include "Engine/MagicBitboardHelper.h"


#define TEST 0

int main() {
	Interface interface;

#if TEST
	//interface.ProcessCommand("position startpos moves e2e4 b8c6 b1c3 g8f6 d2d4 e7e5 d4e5 c6e5 c1f4 e5c6 e4e5 c6e5 f4e5 d8e7 d1e2 e8d8 c3d5 f6d5 e1c1 d5b4 c1b1 e7e6 b2b3 e6f5 g2g4 f5g6 e2c4 c7c5 f1g2 b4c2");
	//interface.ProcessCommand("position startpos moves a2a4 a7a5 b2b4 c7c6 c2c3 d8b6 a1a2 b6f2 e1f2 a8a7 d1b3 c6c5 b3f7 e8d8 f7f8 d8c7 f8f4 d7d6 b4c5 g8f6 c5d6 e7d6 f2e1 h7h5 f4b4 g7g6 b4a5 a7a5 h2h3 b7b5 d2d3 h8h6 g2g4 f6h7 f1g2 h7f8 g2b7 a5a7 b7a8 c7d8 h1h2 d8c7 c1g5 c7d7 a8c6 d7e6 c6h1 b5b4 c3c4 h6h7 g5f6 a7a4 f6a1 a4a7 a1f6 g6g5 f6e7 h7h6 e7d8 e6f7 h1c6 f7g8 b1d2 f8g6 c6h1 g6h8 d8e7 g8f7 d2f1 h6h7 e7f8 f7e8 h1c6 c8d7 f1g3 h7f7 g3h1 f7f1 e1d2 f1f6 h3h4 a7a2 d2e3 a2e2 e3e2 f6f2 e2e1 f2e2 g1e2 e8f7 c6d5 f7g6 d5a8 d6d5 e2g1 d7a4 h2a2 a4d1 d3d4 d1e2 a8b7 h5g4 h4h5 g6f7 b7c8 f7e8 c8d7 e8d8 f8e7 d8e7 d7a4 e2f1 a4e8 e7d6 e8c6 d6e6 c4c5 b8a6 c6e8 a6c5 e1f1 g4g3 h5h6 c5a6 e8d7 e6e7 d7c8 e7f8 a2h2 a6c7 g1e2 c7a8 e2c1 f8e7 c1b3 e7f6 b3a1 f6g6 f1e1 g6h7 c8f5 h8g6 h2e2 b4b3 e2a2 g5g4 a1b3 h7h8 a2a8 h8h7 a8h8 h7h8 b3a1 g6e5 e1e2 e5d3 f5d3 h8g8 h1g3 g8f7 h6h7 f7g7 g3h5 g7h6 h7h8q");
	//interface.ProcessCommand("position startpos");
	//interface.ProcessCommand("show");
	//interface.ProcessCommand("blackattack");
	//interface.ProcessCommand("moves");
	//interface.ProcessCommand("captures");

	//interface.ProcessCommand("position startpos moves g1f3 g8f6 e2e3 e7e5 f3e5 f8d6 e5c4 d6e7 b1c3 e8g8 e3e4 f6e4 c3e4 d7d5 f1d3 d5c4 d3c4 d8d4 d2d3 c8f5 c2c3 d4d7 d1b3 b8c6 b3b7 a7a6 c1f4 a8a7 b7b3 f5e4 d3e4 c6a5 c4f7 f8f7 b3b8");
	//interface.ProcessCommand("go depth 8");
	interface.ProcessCommand("position startpos moves g1f3 g8f6 e2e3 e7e6 d2d4 f8b4 c1d2 b8c6 d2b4 c6b4 d1d2 d8e7 a2a3 b4c6 b1c3 e8g8 f1b5 a7a6 b5c6 d7c6 e3e4 c6c5 e4e5 f6d5 e1g1 b7b6 c3e4 c5c4 f1e1 e7d7 d2e2 d7a4 a1d1 c8b7 h2h3 a8d8 e2d2 a4b5 d1b1 b5d7 d2e2 d7c6 b1d1 c4c3 b2c3 d5c3 e4c3 c6c3 d1d3 c3c6 e2d2 f7f5 d3c3 c6d7 d2f4 h7h6 c3d3 d7a4 e1e2 a4b5 e2d2 b5b1 g1h2 b7e4 d3c3 d8c8 c3b3 b1c1 b3c3 c1a1 c3e3 g7g5 f3g5 h6g5 f4g5 g8f7 g5f6 f7e8 f6e6 e8d8 e6h6 d8e7 h6h4 e7f7 h4h7 f7e6 e3b3 a1c1 h7h6 e6e7 h6g5 e7d7 g5g7 d7c6 b3c3 c6b7 g7h6 f5f4 d2e2 e4g2 c3c7 c8c7 e2e1 c1e1 h2g2 f4f3 g2g3 f8g8 g3h4 e1e4 h4h5");
	interface.ProcessCommand("show");
	interface.ProcessCommand("go depth 7");

#else
	interface.ListenForConnection();
	interface.ListenForCommands();
#endif

	return 0;
}