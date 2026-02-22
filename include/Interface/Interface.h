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

private:

	bool Position(std::vector<std::string>& words);
	bool Go(std::vector<std::string>& words);
	bool Perft(std::vector<std::string>& words);

	Board m_board;
	MoveGenerator m_moveGenerator;
	Player m_player;
};