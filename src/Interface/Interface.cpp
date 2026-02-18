#include "Interface/Interface.h"

Interface::Interface() :
	m_board{ Board() },
	m_moveGenerator{ m_board },
	m_player{ m_board }
{}

void Interface::ListenForConnection() {
	while (true) {
		std::string input;
		std::getline(std::cin, input);

		std::cerr << "Log: Message received {" << input << "}\n";

		if (input == "uci") {
			time_t connectionTime = time(nullptr);
			std::cerr << "Log: Connection opened at " << ctime(&connectionTime) << '\n';

			std::cout << "id name " << ENGINE_NAME << '\n';
			std::cout << "id author " << AUTHOR << '\n';
			std::cout << "uciok\n";
			break;
		} else {
			std::cerr << "Log: Input was unexpected";
		}
	}
}

void Interface::ListenForCommands() {
	while (true) {
		//std::cerr << m_board;

		std::string input;
		std::getline(std::cin, input);

		std::cerr << "Log: Message received {" << input << "}\n";

		if (input == "") {
			continue;
		}

		if (input == "quit") {
			time_t connectionTime = time(nullptr);
			std::cerr << "Log: Connection closed at " << ctime(&connectionTime) << '\n';
			return;
		}

		if (input == "ucinewgame") {
			m_board.Initialise();
			continue;
		}

		if (input == "isready") {
			std::cout << "readyok\n";
			continue;
		}

		if (input == "stop") {
			std::cerr << "Log: stop called. Currently does nothing\n";
			continue;
		}

		if (input == "ponderhit") {
			std::cerr << "Log: ponderhit called. Currently does nothing\n";
		}

		std::stringstream ss(input);
		std::vector<std::string> words;
		std::string word;
		while (getline(ss, word, ' '))
			words.push_back(word);

		if (words.at(0) == "position") {
			if (!SetUpPosition(words))
				std::cerr << "Log: Position setup failed\n";
			continue;
		}

		if (words.at(0) == "go") {
			if (!Go(words))
				std::cerr << "Log: Go failed\n";
			continue;
		}

		std::cerr << "Log: input fell through...\n";
	}
}

bool Interface::SetUpPosition(std::vector<std::string>& words) {
	if (words.at(1) == "fen") {
		std::cerr << "Log: fen position indicated. Currently not supported\n";
		return false;
	} else if (words.at(1) == "startpos") {
		m_board.SetUpStartPosition();
		if (words.size() <=2)
			return true;
	} else {
		std::cerr << "Log: Invalid followup to position\n";
		return false;
	}

	if (words.at(2) != "moves") {
		return false;
	}

	for (int i = 3; i < words.size(); ++i) {
		std::string& sRequestedMove = words.at(i);
		std::vector<Move> legalMoves = m_moveGenerator.GenerateLegalMoves();

		bool foundMove = false;
		for (const Move& move : legalMoves) {
			std::string sMove = move.ToString();
			if (sRequestedMove == sMove) {
				std::cerr << "Log: Making move " << sMove << '\n';
				m_board.MakeMove(move);
				foundMove = true;
				break;
			}
		}

		if (!foundMove) {
			std::cerr << "Log: Unexpected move {" << sRequestedMove << "}\n";
			return false;
		}
	}

	std::cerr << m_board;

	return true;
}

bool Interface::Go(std::vector<std::string>& words) {
	if (words.at(1) == "depth") {
		int depth = std::stoi(words.at(2));
		const Move& move = m_player.GoDepth(depth);
		std::cout << "bestmove " << move.ToString() << '\n';
		return true;
	} else {
		std::cerr << "Log: go option not understood\n";
	}

	return false;
}