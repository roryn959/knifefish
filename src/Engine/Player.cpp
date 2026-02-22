#include "Engine/Player.h"

//./cutechess-cli -engine cmd=./engines/basic_negamax name=Player1 proto=uci stderr=player1_logs -engine cmd=./engines/basic_negamax name=Player2 proto=uci stderr=player2_logs -each tc=inf depth=4 -games 1 -pgnout games.pgn


Player::Player(Board& board) :
#if DEBUG
	m_nodesSearched{0},
#endif
	m_board{board},
	m_moveGenerator{m_board}
{}

Move Player::GoDepth(int depth) {
	return IterativeDeepening(depth);
}

int Player::Evaluate() {
	int eval = 0;

	Bitboard piecePositions;

	#define X(piece) 																	\
																						\
	piecePositions = m_board.GetPieceBitboard(Piece::piece);							\
	for (Square piecePosition : piecePositions) { 										\
		eval += PIECE_VALUES[Piece::piece];												\
		eval -= pieceSquareTables[Piece::piece][static_cast<size_t>(piecePosition)];	\
	}

	PIECES_LIST
	#undef X

	return m_board.IsWhiteTurn() ? eval : -eval;
}

Move Player::IterativeDeepening(int maxDepth) {
	// Initialise garbage move that will never clash with anything
	Move bestMove{ A1_MASK, A1_MASK };

	int depth = 1;
	while (depth <= maxDepth) {
		auto startTime = std::chrono::system_clock::now();

		bestMove = RootNegamax(depth, bestMove);

		auto endTime = std::chrono::system_clock::now();

		++depth;

#if DEBUG
		auto searchTime = endTime - startTime;
		auto searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(searchTime).count();
		auto searchTimeS = searchTimeMs / 1000.0;

		std::cerr << "Log: Nodes searched: " << m_nodesSearched << '\n';
		std::cerr << "Log: Search speed (nps): " << m_nodesSearched / searchTimeS << '\n';
		std::cerr << "Log: Search time (s): " << searchTimeS << "\n\n";
#endif
	}

	return bestMove;
}

Move Player::RootNegamax(int depth, const Move& movePv) {
#if DEBUG
	m_nodesSearched = 1;
	std::cerr << "Log: Calling root Negamax with depth " << depth << '\n';
#endif

	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();

	// Reorder with PV move first
	for (int i = 0; i < moves.size(); ++i) {
		if (moves[i] == movePv) {
			Move temp = moves[0];
			moves[0] = moves[i];
			moves[i] = temp;
			break;
		}
	}

	Move bestMove;
	int bestScore = -1'000'000;
	int alpha = -1'000'000;
	int beta = 1'000'000;

	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		int score = -Negamax(depth - 1, -beta, -alpha);
		m_board.UndoMove(move, undo);

		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
			if (score > alpha) {
				alpha = score;
			}
		}
	}

#if DEBUG
	std::cerr << "Log: Negamax depth " << depth << " returned an evaluation of " << bestScore << "\n";
	std::cerr << "Log: Negamax " << depth << " found the best move to be " << bestMove;
#endif

	return bestMove;
}

bool Player::IsCheckmate() {
	if (m_board.IsWhiteTurn())
		return m_board.IsAttackedByBlack(m_board.GetPieceBitboard(Piece::WHITE_KING));
	return m_board.IsAttackedByWhite(m_board.GetPieceBitboard(Piece::BLACK_KING));
}

int Player::Negamax(int depth, int alpha, int beta) {
#if DEBUG
	++m_nodesSearched;
#endif

	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();

	if (moves.size() == 0) {
		if (IsCheckmate())
			return -100'000;
		return 0;
	}

	if (depth == 0)
		return Evaluate();

	int bestScore = -100'000;

	for (const Move& move : moves) {
		// In this context, alpha is the score we (max) are currently guaranteed. Beta is the score the other guy (min) is guaranteed.
		// So when we call deeper, we swap them and negate them because the role of maximiser is switched, and the scores should be viewed from the other way round.
		Undo undo = m_board.MakeMove(move);
		int score = -Negamax(depth - 1, -beta, -alpha);
		m_board.UndoMove(move, undo);

		if (score > bestScore) {
			bestScore = score;
			if (score > alpha) {
				alpha = score;
			}
		}

		// The score we (max) get is better for us than the score the other guy (min) is already guaranteed, so he will never take it! Stop searching
		if (score >= beta) {
			break;
		}
	}
	return bestScore;
}

int Player::RootPerft(int depth) {
#if DEBUG
	auto startTime = std::chrono::system_clock::now();
#endif

	if (depth == 0)
		return 1;

	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();

	if (moves.size() == 0)
		return 0;
	
	int overallTotal = 0;
	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		int total = Perft(depth - 1);
		m_board.UndoMove(move, undo);

		overallTotal += total;

		std::cout << move.ToString() << ": " << total << '\n';
	}


#if DEBUG
	auto endTime = std::chrono::system_clock::now();
	auto searchTime = endTime - startTime;
	auto searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(searchTime).count();
	auto searchTimeS = searchTimeMs / 1000.0;

	std::cerr << "Log: Nodes searched: " << m_nodesSearched << '\n';
	std::cerr << "Log: Search time (s): " << searchTimeS << '\n';
	std::cerr << "Log: Search speed (nps): " << m_nodesSearched / searchTimeS << '\n';
#endif

	return overallTotal;
}

int Player::Perft(int depth) {
	if (depth == 0)
		return 1;

	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();

	if (moves.size() == 0)
		return 0;
	
	int total = 0;
	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		total += Perft(depth - 1);
		m_board.UndoMove(move, undo);
	}

	return total;
}