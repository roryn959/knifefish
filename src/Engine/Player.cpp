#include "Engine/Player.h"


Player::Player(Board& board) :
	m_board{board},
	m_moveGenerator{m_board},
	m_transpositionTable{},
	m_nodesSearched{0},
	m_deadline{},
	m_isStopped{false}
{}

Move Player::Go(int depth, int wtime, int btime, int winc, int binc, int movestogo, int movetime) {
	Moment startTime = Clock::now();

	if (depth <= 0)
		depth = 15;

	float timeAllowedSecs;

	if (movetime > 0) {
		timeAllowedSecs = movetime / 1000.0f;
	} else if (m_board.IsWhiteTurn() && wtime > 0) {
		float whiteTimeSecs = wtime / 1000.0f;
		float whiteIncrementSecs = winc / 1000.0f;
		float baseTimeSecs = (whiteTimeSecs / 30.0f) + whiteIncrementSecs;
		timeAllowedSecs = std::min(baseTimeSecs, whiteTimeSecs / 2.0f);
	} else if (btime > 0) {
		float blackTimeSecs = btime / 1000.0f;
		float blackIncrementSecs = binc / 1000.0f;
		float baseTimeSecs = (blackTimeSecs / 30.0f) + blackIncrementSecs;
		timeAllowedSecs = std::min(baseTimeSecs, blackTimeSecs / 2.0f);
	} else {
		timeAllowedSecs = 600;
	}

	std::cerr << "Choosing to spend " << timeAllowedSecs << "s on this move.\n";

	m_deadline = startTime + ms(SecsToMillisecs(timeAllowedSecs));
	Move bestMove = IterativeDeepening(depth);

#if DEBUG
	auto searchTime = Clock::now() - startTime;
	auto searchTimeS = std::chrono::duration_cast<ms>(searchTime).count() / 1000.0;
	std::cerr << "Log: Nodes searched: " << m_nodesSearched << '\n';
	std::cerr << "Log: Search speed (nps): " << m_nodesSearched / searchTimeS << '\n';
	std::cerr << "Log: Transpositions hit: " << m_transpositionsHit << '\n';
	std::cerr << "Log: Search time (s): " << searchTimeS << "\n\n";
#endif

	std::cerr << "Returning move " << bestMove << '\n';
	return bestMove;
}

int16_t Player::Evaluate() {
	int16_t eval = 0;

	Bitboard piecePositions;

	#define X(piece) 																			\
																								\
	piecePositions = m_board.GetPieceBitboard(Piece::piece);									\
	for (Square piecePosition : piecePositions) { 												\
		eval += PIECE_VALUES[Piece::piece];														\
		eval -= midgamePieceSquareTables[Piece::piece][static_cast<size_t>(piecePosition)];		\
	}
	PIECES_LIST
	#undef X

	return m_board.IsWhiteTurn() ? eval : -eval;
}

Move Player::IterativeDeepening(int8_t maxDepth) {
	m_nodesSearched = 0;
	m_isStopped = false;

	// Initialise PV move to garbage move that will never clash with anything
	Move movePv{ Square::a1, Square::a1 };
	int16_t bestScore = -MAX_SCORE;
	int8_t depth = 1;

	while (depth <= maxDepth) {
		Move bestMove;
		int16_t score = RootNegamax(depth, movePv, bestMove);

		if (m_isStopped)
			break;

		if (score > bestScore) {
			movePv = bestMove;
		}

		++depth;
	}

	return movePv;
}

int16_t Player::RootNegamax(int8_t depth, const Move& movePv, Move& bestMove) {
#if DEBUG
	m_transpositionsHit = 0;
	std::cerr << "Log: Called root Negamax with depth " << (int) depth << '\n';
#endif

	++m_nodesSearched;

	MoveList moves;
	bool check = m_moveGenerator.GenerateMoves(moves);

	if (moves.size() == 0) {
		return check ? -MATE_SCORE : 0;
	}

	// Reorder with PV move first
	for (int i = 0; i < moves.size(); ++i) {
		if (moves[i] == movePv) {
			Move temp = moves[0];
			moves[0] = moves[i];
			moves[i] = temp;
			break;
		}
	}

	int16_t bestScore = -MAX_SCORE;
	int16_t alpha = -MAX_SCORE;
	int16_t beta = MAX_SCORE;

	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		int16_t score = -Negamax(depth - 1, -beta, -alpha);
		m_board.UndoMove(move, undo);

		if (m_isStopped)
			return 0;

		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
			if (score > alpha) {
				alpha = score;
			}
		}
	}

#if DEBUG
	std::cerr << "Log: Negamax depth " << (int) depth << " returned an evaluation of " << bestScore << "\n";
	std::cerr << "Log: Negamax " << (int) depth << " found the best move to be " << bestMove << '\n';
#endif

	return bestScore;
}

int16_t Player::Negamax(int8_t depth, int16_t alpha, int16_t beta) {

	if (((m_nodesSearched % 2048) == 0) && (Clock::now() >= m_deadline)) {
		m_isStopped = true;
		return 0;
	}

	++m_nodesSearched;

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

	MoveList moves;
	bool check = m_moveGenerator.GenerateMoves(moves);

	if (moves.size() == 0) {
		return check ? -MATE_SCORE : 0;
	}

	if (depth == 0)
		return Quiescence(10, alpha, beta);

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

int16_t Player::Quiescence(int8_t depth, int16_t alpha, int16_t beta) {
	int16_t eval = Evaluate();

	if (depth == 0 || eval >= beta)
		return beta;

	if (eval > alpha)
		alpha = eval;

	MoveList captures;
	m_moveGenerator.GenerateMoves(captures, true);

	for (const Move& capture : captures) {
		Undo undo = m_board.MakeMove(capture);
		int16_t score = -Quiescence(depth - 1,-beta, -alpha);
		m_board.UndoMove(capture, undo);

		if (score >= beta)
			return beta;
		
		if (score > alpha)
			alpha = score;
	}

	return alpha;
}

int Player::RootPerft(int8_t depth) {
#if DEBUG
	auto startTime = std::chrono::system_clock::now();
#endif

	if (depth == 0)
		return 1;

	MoveList moves;
	m_moveGenerator.GenerateMoves(moves);

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

	std::cerr << "Log: Search time (s): " << searchTimeS << '\n';
	std::cerr << "Log: Nodes per second: " << overallTotal / searchTimeS << '\n';
#endif

	return overallTotal;
}

int Player::Perft(int8_t depth) {
	if (depth == 0)
		return 1;

	MoveList moves;
	m_moveGenerator.GenerateMoves(moves);

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