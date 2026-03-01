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

		if (!ProcessCommand(input))
			break;
	}
}

bool Interface::ProcessCommand(std::string input) {
	if (input == "") {
		return true;
	}

	if (input == "show") {
		std::cout << m_board;
		return true;
	}

	if (input == "moves") {
		std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();
		for (const Move& m : moves) {
			std::cout << m;
		}
		return true;
	}

	if (input == "pseudomoves") {
		std::vector<Move> moves = m_moveGenerator.GeneratePseudoMoves();
		for (const Move& m : moves) {
			std::cout << m;
		}
		return true;
	}

	if (input == "blackattack") {
		std::cout << m_moveGenerator.GetBlackAttackSet();
		return true;
	}

	if (input == "whiteattack") {
		std::cout << m_moveGenerator.GetWhiteAttackSet();
		return true;
	}

	if (input == "eval") {
		std::cout << m_player.Evaluate() << '\n';
		return true;
	}

	if (input == "hash") {
		std::cout << m_board.GetHash() << '\n';
		return true;
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
		return true;
	}

	if (input == "quit") {
		time_t connectionTime = time(nullptr);
		std::cerr << "Log: Connection closed at " << ctime(&connectionTime) << '\n';
		return false;
	}

	if (input == "ucinewgame") {
		return true;
	}

	if (input == "isready") {
		std::cout << "readyok\n" << std::flush;
		return true;
	}

	if (input == "stop") {
		std::cerr << "Log: stop called. Currently does nothing\n";
		return true;
	}

	if (input == "ponderhit") {
		std::cerr << "Log: ponderhit called. Currently does nothing\n";
	}

	std::istringstream tokenStream(input);
	std::string token;

	tokenStream >> token;

	if (token == "position") {
		if (!Position(tokenStream))
			std::cerr << "Log: Position setup failed\n";
		return true;
	}

	if (token == "go") {
		if (!Go(tokenStream))
			std::cerr << "Log: Go failed\n";
		return true;
	}

	if (token == "perft") {
		if (!Perft(tokenStream))
			std::cerr << "Log: Perft failed\n";
		return true;
	}

	std::cerr << "Log: input fell through...\n";
	return true;
}

bool Interface::Position(std::istringstream& tokenStream) {
	std::string token;
	tokenStream >> token;

	if (token == "fen") {
		std::cerr << "Log: fen position indicated. Currently not supported\n";
		return false;
	} else if (token == "startpos") {
		m_board.SetUpStartPosition();
	} else {
		std::cerr << "Log: Unrecognised position reference.\n";
		return false;
	}

	if (!(tokenStream >> token))
		return true;

	if (token != "moves")
		return false;

	while (tokenStream >> token) {
		std::vector<Move> legalMoves = m_moveGenerator.GenerateLegalMoves();

		bool foundMove = false;
		for (const Move& move : legalMoves) {
			if (token == move.ToString()) {
				m_board.MakeMove(move);
				foundMove = true;
				break;
			}
		}

		if (!foundMove) {
			std::cerr << "Log: Illegal move {" << token << "}\n";
			return false;
		}
	}

	return true;
}

bool Interface::Go(std::istringstream& tokenStream) {
	int depth = -1;
	int wtime = -1;
	int btime = -1;
	int winc = -1;
	int binc = -1;
	int movestogo = -1;
	int movetime = -1;

	std::string token;
	
	while (tokenStream >> token) {
		if (token == "depth") tokenStream >> depth;
		else if (token == "wtime") tokenStream >> wtime;
		else if (token == "btime") tokenStream >> btime;
		else if (token == "winc") tokenStream >> winc;
		else if (token == "binc") tokenStream >> binc;
		else if (token == "movestogo") tokenStream >> movestogo;
		else if (token == "movetime") tokenStream >> movetime;
		else {
			std::cout << "Error: Unrecognised option {" << token << "}.\n";
			return false;
		}
	}

	Move bestMove = m_player.Go(depth, wtime, btime, winc, binc, movestogo, movetime);
	std::cout << "bestmove " << bestMove.ToString() << '\n' << std::flush;

	return true;
}

bool Interface::Perft(std::istringstream& tokenStream) {
	int depth = -1; 
	if (!(tokenStream >> depth)) {
		std::cout << "Error: something went wrong. Check depth was included.\n";
		return false;
	}

	if (depth <= 0) {
		std::cout << "Error: Perft depth must be at least 1.\n";
		return false;
	}

	int totalMoves = m_player.RootPerft(depth);
	std::cout << "Total: " << totalMoves << '\n' << std::flush;

	return true;
}