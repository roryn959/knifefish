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
			std::cerr << "Log: Input was unexpected\n";
		}
	}
}

void Interface::ListenForCommands() {
	while (true) {
		std::string input;
		std::getline(std::cin, input);

		std::cerr << "Log: Message received {" << input << "}\n";

		if (input == "") {
			continue;
		}

		if (input == "show") {
			std::cout << m_board;
			continue;
		}

		if (input == "moves") {
			std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();
			for (const Move& m : moves) {
				std::cout << m;
			}
			continue;
		}

		if (input == "blackattack") {
			std::cout << m_board.GetBlackAttackSet();
			continue;
		}

		if (input == "whiteattack") {
			std::cout << m_board.GetWhiteAttackSet();
			continue;
		}

		if (input == "eval") {
			std::cout << m_player.Evaluate() << '\n';
			continue;
		}

		if (input == "hash") {
			std::cout << m_board.GetHash() << '\n';
			continue;
		}

		if (input == "rebuildhash") {
			Hash oldHash = m_board.GetHash();
			m_board.RebuildHash();
			Hash newHash = m_board.GetHash();
			if (oldHash != newHash) {
				std::cerr << "Error: Hash inconsistency found on hash rebuild.\n";
				std::exit(1);
			}
			std::cout << newHash << '\n';
			continue;
		}

		if (input == "quit") {
			time_t connectionTime = time(nullptr);
			std::cerr << "Log: Connection closed at " << ctime(&connectionTime) << '\n';
			return;
		}

		if (input == "ucinewgame") {
			continue;
		}

		if (input == "isready") {
			std::cout << "readyok\n" << std::flush;
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
			if (!Position(words))
				std::cerr << "Log: Position setup failed\n";
			continue;
		}

		if (words.at(0) == "go") {
			if (!Go(words))
				std::cerr << "Log: Go failed\n";
			continue;
		}

		if (words.at(0) == "perft") {
			if (!Perft(words))
				std::cerr << "Log: Perft failed\n";
			continue;
		}

		std::cerr << "Log: input fell through...\n";
	}
}

bool Interface::Position(std::vector<std::string>& words) {
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
				m_board.MakeMove(move);
				foundMove = true;
				break;
			}
		}

		if (!foundMove) {
			std::cerr << "Log: Illegal move {" << sRequestedMove << "}\n";
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
		std::cout << "bestmove " << move.ToString() << '\n' << std::flush;
		return true;
	} else {
		std::cerr << "Log: go option not understood\n";
	}

	return false;
}

bool Interface::Perft(std::vector<std::string>& words) {
	int depth = std::stoi(words.at(1));
	int totalMoves = m_player.RootPerft(depth);
	std::cout << "Total: " << totalMoves << '\n' << std::flush;

	return true;
}