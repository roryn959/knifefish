#include <iostream>

#include "Interface/Interface.h"

int main() {
	Interface interface;

//	interface.ProcessCommand("ucinewgame");
//	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 e1g1 b7b5 a4b3 f8e7 d2d4 e8g8 d4e5 f6g4 b3d5 a8b8 c1f4 c6b4 h2h3 c7c6 d5b3 g7g5 f4g3 d8b6 h3g4 f8e8 a2a3 h7h6 a3b4 e7b4 e5e6 d7e6 g3b8 e8d8");
//	interface.ProcessCommand("go wtime 126023 btime 106630 winc 2000 binc 2000");
//	interface.ProcessCommand("position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 e1g1 b7b5 a4b3 f8e7 d2d4 e8g8 d4e5 f6g4 b3d5 a8b8 c1f4 c6b4 h2h3 c7c6 d5b3 g7g5 f4g3 d8b6 h3g4 f8e8 a2a3 h7h6 a3b4 e7b4 e5e6 d7e6 g3b8 e8d8 b8a7 b6c7");
//	interface.ProcessCommand("go wtime 126023 btime 106630 winc 2000 binc 2000");
//	interface.ProcessCommand("go depth 10");
//	interface.ProcessCommand("show");

//	return 0;

	interface.ListenForConnection();
	interface.ListenForCommands();

	return 0;
}