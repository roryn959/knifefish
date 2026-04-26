#pragma once

#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

#include "BoardRepresentation/Board.h"

#include "Engine/Move.h"
#include "Engine/MoveGenerator.h"
#include "Engine/Player.h"

#define ENGINE_NAME "Knifefish"
#define AUTHOR "Rory"


class Interface {
public:
	Interface();

	void ListenForConnection();
	void ListenForCommands();

	bool ProcessCommand(std::string input);

private:
	bool NewGame();
	bool Position(std::istringstream& tokenStream);
	bool StartPosition(std::istringstream& tokenStream);
	bool FenPosition(std::istringstream& tokenStream);
	bool Go(std::istringstream& tokenStream);
	bool Perft(std::istringstream& tokenStream);

	void FlushCommandHistory();

	Board 						m_board;
	MoveGenerator 				m_moveGenerator;
	Player 						m_player;

	std::vector<std::string> 	m_commandHistory;
};