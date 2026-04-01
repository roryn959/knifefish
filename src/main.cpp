#include <iostream>

#include "Interface/Interface.h"

#include "BoardRepresentation/Bitboard.h"
#include "Engine/MagicBitboardHelper.h"


#define TEST 0

int main() {
	Interface interface;

#if TEST
	//interface.ProcessCommand("position startpos moves e2e4 b8c6 b1c3 g8f6 d2d4 e7e5 d4e5 c6e5 c1f4 e5c6 e4e5 c6e5 f4e5 d8e7 d1e2 e8d8 c3d5 f6d5 e1c1 d5b4 c1b1 e7e6 b2b3 e6f5 g2g4 f5g6 e2c4 c7c5 f1g2 b4c2");

	//interface.ProcessCommand("position startpos moves g1f3 g8f6 e2e3 e7e5 f3e5 f8d6 e5c4 d6e7 b1c3 e8g8 e3e4 f6e4 c3e4 d7d5 f1d3 d5c4 d3c4 d8d4 d2d3 c8f5 c2c3 d4d7 d1b3 b8c6 b3b7 a7a6 c1f4 a8a7 b7b3 f5e4 d3e4 c6a5 c4f7 f8f7 b3b8");
	//interface.ProcessCommand("go depth 8");
	//interface.ProcessCommand("position startpos moves e2e4 e7e5 b1c3 b8c6 c3b1 c6b8");
	//interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6 f3g5 d8g5 d2d4 g5f5 f2f4 e5e4 b1d2 c8e6");
	//interface.ProcessCommand("go wtime 88979 btime 137720 winc 2000 binc 2000");
	//interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6 f3g5 d8g5 d2d4 g5f5 f2f4 e5e4 b1d2 c8e6 d2e4 e8c8");
	//interface.ProcessCommand("show");
	//interface.ProcessCommand("go wtime 86190 btime 139690 winc 2000 binc 2000");
	interface.ProcessCommand("ucinewgame");
	interface.ProcessCommand("isready");
	interface.ProcessCommand("position startpos");
	interface.ProcessCommand("go movetime 10000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5");
	interface.ProcessCommand("go wtime 118999 btime 120000 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6");
	interface.ProcessCommand("go wtime 115200 btime 121960 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6");
	interface.ProcessCommand("go wtime 111520 btime 123930 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4");
	interface.ProcessCommand("go wtime 107970 btime 125900 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5");
	interface.ProcessCommand("go wtime 104540 btime 127870 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6");
	interface.ProcessCommand("go wtime 101220 btime 129840 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6 f3g5 d8g5");
	interface.ProcessCommand("go wtime 98010 btime 131810 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6 f3g5 d8g5 d2d4 g5f5");
	interface.ProcessCommand("go wtime 94890 btime 133780 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6 f3g5 d8g5 d2d4 g5f5 f2f4 e5e4");
	interface.ProcessCommand("go wtime 91880 btime 135750 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6 f3g5 d8g5 d2d4 g5f5 f2f4 e5e4 b1d2 c8e6");
	interface.ProcessCommand("go wtime 88979 btime 137720 winc 2000 binc 2000");
	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d1e2 e4g5 b5c6 d7c6 f3g5 d8g5 d2d4 g5f5 f2f4 e5e4 b1d2 c8e6 d2e4 e8c8");
	interface.ProcessCommand("go wtime 86190 btime 139690 winc 2000 binc 2000");


#else
	interface.ListenForConnection();
	interface.ListenForCommands();
#endif

	return 0;
}