#include "Engine/Player.h"

//./cutechess-cli -engine cmd=./engines/basic_negamax name=Player1 proto=uci stderr=player1_logs -engine cmd=./engines/basic_negamax name=Player2 proto=uci stderr=player2_logs -each tc=inf depth=4 -games 1 -pgnout games.pgn


Player::Player(Board& board) :
#if DEBUG
	m_nodesSearched{0},
#endif
	m_board{board},
	m_moveGenerator{m_board},
	m_transpositionTable{}
{}

Move Player::Go(int depth, int wtime, int btime, int winc, int binc, int movestogo, int movetime) {
	Moment startTime = Clock::now();

	// Set defaults (or otherwise make decisions).
	if (depth <= 0) depth = 10;
	int timeAllowedSecs = 10;

	Moment deadline = startTime + ms(SecsToMillisecs(timeAllowedSecs));
	Move bestMove = IterativeDeepening(depth, deadline);

#if DEBUG
	auto searchTime = Clock::now() - startTime;
	auto searchTimeS = std::chrono::duration_cast<ms>(searchTime).count() / 1000.0;
	std::cerr << "Log: Nodes searched: " << m_nodesSearched << '\n';
	std::cerr << "Log: Search speed (nps): " << m_nodesSearched / searchTimeS << '\n';
	std::cerr << "Log: Transpositions hit: " << m_transpositionsHit << '\n';
	std::cerr << "Log: Search time (s): " << searchTimeS << "\n\n";
#endif

	return bestMove;
}

int16_t Player::Evaluate() {
	int16_t eval = 0;

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

Move Player::IterativeDeepening(int8_t maxDepth, Moment timeDeadline) {
	// Initialise PV move to garbage move that will never clash with anything
	Move movePv{ A1_MASK, A1_MASK };
	int16_t bestScore = -MAX_SCORE;
	int8_t depth = 1;

	while (depth <= maxDepth) {
		if (Clock::now() >= timeDeadline)
			break;

		Move bestMove;
		int16_t score = RootNegamax(depth, movePv, bestMove, timeDeadline);
		if (score > bestScore) {
			movePv = bestMove;
		}

		++depth;
	}

	return movePv;
}

int16_t Player::RootNegamax(int8_t depth, const Move& movePv, Move& bestMove, Moment deadline) {
#if DEBUG
	m_nodesSearched = 1;
	m_transpositionsHit = 0;
	std::cerr << "Log: Called root Negamax with depth " << (int) depth << '\n';
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

	int16_t bestScore = -MATE_SCORE;
	int16_t alpha = -MAX_SCORE;
	int16_t beta = MAX_SCORE;

	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		int16_t score = -Negamax(depth - 1, -beta, -alpha);
		m_board.UndoMove(move, undo);

		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
			if (score > alpha) {
				alpha = score;
			}
		}

		if (Clock::now() >= deadline)
			break;
	}

#if DEBUG
	std::cerr << "Log: Negamax depth " << depth << " returned an evaluation of " << bestScore << "\n";
	std::cerr << "Log: Negamax " << depth << " found the best move to be " << bestMove;
#endif

	return bestScore;
}

bool Player::IsCheckmate() {
	// This function assumes it is called from a context where there are no possible moves for the side to play
	if (m_board.IsWhiteTurn())
		return m_board.IsAttackedByBlack(m_board.GetPieceBitboard(Piece::WHITE_KING));
	return m_board.IsAttackedByWhite(m_board.GetPieceBitboard(Piece::BLACK_KING));
}

int16_t Player::Negamax(int8_t depth, int16_t alpha, int16_t beta) {
	Hash hash = m_board.GetHash();
	const TranspositionTableEntry* pEntry = m_transpositionTable.GetEntry(hash);

	if ((pEntry != nullptr) && (pEntry->m_depth >= depth)) {
		switch (pEntry->m_evaluationType) {
			case EvaluationType::EXACT: {
#if DEBUG
				++m_transpositionsHit;
#endif
				return pEntry->m_score;
			}
			case EvaluationType::LOWER_BOUND: {
				if (pEntry->m_score >= beta)
					return pEntry->m_score;
				alpha = std::max(alpha, pEntry->m_score);
				break;
			}
			case EvaluationType::UPPER_BOUND: {
				if (pEntry->m_score <= alpha)
					return pEntry->m_score;
				beta = std::min(beta, pEntry->m_score);
				break;
			}
		}
	}

#if DEBUG
	++m_nodesSearched;
#endif

	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();

	if (moves.size() == 0) {
		if (IsCheckmate())
			return -MATE_SCORE;
		return 0;
	}

	if (depth == 0)
		return Evaluate();

	int16_t bestScore = -MAX_SCORE;
	Move bestMove;
	EvaluationType evaluationType = EvaluationType::UPPER_BOUND;

	for (const Move& move : moves) {
		// In this context, alpha is the score we (max) are currently guaranteed. Beta is the score the other guy (min) is guaranteed.
		// So when we call deeper, we swap them and negate them because the role of maximiser is switched, and the scores should be viewed from the other way round.
		Undo undo = m_board.MakeMove(move);
		int16_t score = -Negamax(depth - 1, -beta, -alpha);
		m_board.UndoMove(move, undo);

		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
			if (bestScore > alpha) {
				alpha = bestScore;
				evaluationType = EvaluationType::EXACT;
			}

		}

		// The score we (max) get is better for us than the score the other guy (min) is already guaranteed, so he will never take it! Stop searching
		if (score >= beta) {
			evaluationType = EvaluationType::LOWER_BOUND;
			break;
		}
	}

	TranspositionTableEntry entry {
		hash,
		bestScore,
		depth,
		evaluationType
	};

	m_transpositionTable.SetEntry(hash, entry);

	return bestScore;
}

int Player::RootPerft(int8_t depth) {
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

int Player::Perft(int8_t depth) {
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