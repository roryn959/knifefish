#include "Engine/Player.h"


Player::Player(Board& board) :
	m_board{board},
	m_moveGenerator{m_board},
	m_transpositionTable{},
	m_killers{},
	m_nodesSearched{0},
	m_deadline{},
	m_isStopped{false}
{
	InitialisePieceSquareTables();
}

void Player::InitialisePieceSquareTables() {
	#define X(piece) 																																									\
	for (size_t i = static_cast<size_t>(Square::h1); i < static_cast<size_t>(Square::COUNT); ++i) {																						\
		m_midgamePieceSquareTables[static_cast<size_t>(Piece::piece)][i] = MG_PIECE_VALUES[static_cast<size_t>(Piece::piece)] - MG_PST_LIST[static_cast<size_t>(Piece::piece)][i]; 		\
		m_endgamePieceSquareTables[static_cast<size_t>(Piece::piece)][i] = EG_PIECE_VALUES[static_cast<size_t>(Piece::piece)] - EG_PST_LIST[static_cast<size_t>(Piece::piece)][i];		\
	}
	PIECES_LIST
	#undef X
}

Move Player::Go(int depth, int wtime, int btime, int winc, int binc, int movestogo, int movetime) {
	Moment startTime = Clock::now();

	if (depth <= 0)
		depth = MAX_DEPTH;

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

	// Hack to avoid losing on time due to lag: shave off 100 ms from the allotted time.
	if (timeAllowedSecs > 0.1)
		timeAllowedSecs -= 0.1;

	std::cerr << "Spending " << timeAllowedSecs << "s on this move.\n\n";

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

	return bestMove;
}

int16_t Player::Evaluate() {
	int eval = 0;

	int mg_eval = 0;
	int eg_eval = 0;

	#define X(piece) 																									\
	for (Square piecePosition : m_board.GetPieceBitboard(Piece::piece)) { 												\
		mg_eval += m_midgamePieceSquareTables[static_cast<size_t>(Piece::piece)][static_cast<size_t>(piecePosition)];	\
		eg_eval += m_endgamePieceSquareTables[static_cast<size_t>(Piece::piece)][static_cast<size_t>(piecePosition)];	\
	}
	PIECES_LIST
	#undef X

	Square whiteKingSquare = static_cast<Square>(m_board.GetPieceBitboard(Piece::WHITE_KING));
	Bitboard whiteKingDefenders = m_board.GetPieceBitboard(Piece::WHITE_PAWN) & WHITE_KING_DEFENDERS_MASK & WHITE_KING_DEFENCE_MASKS[static_cast<size_t>(whiteKingSquare)];

	for (Square sq : whiteKingDefenders)
		mg_eval += KING_DEFENCE_PAWN_PST[static_cast<size_t>(sq)];

	Square blackKingSquare = static_cast<Square>(m_board.GetPieceBitboard(Piece::BLACK_KING));
	Bitboard blackKingDefenders = m_board.GetPieceBitboard(Piece::BLACK_PAWN) & BLACK_KING_DEFENDERS_MASK & BLACK_KING_DEFENCE_MASKS[static_cast<size_t>(blackKingSquare)];

	for (Square sq : blackKingDefenders)
		mg_eval -= KING_DEFENCE_PAWN_PST[static_cast<size_t>(sq)];

	int phase = m_board.GetPhase();

	eval += mg_eval * phase;
	eval += eg_eval * (START_PHASE - phase);

	eval /= START_PHASE;

	if (!m_board.IsWhiteTurn())
		eval *= -1;

	return static_cast<int16_t>(eval);
}

bool Player::CheckForPotentialRepetition(int8_t depth) {
	if (depth == 0)
		return m_board.CheckQuietDraws();

	MoveList moves;
	MoveGenerationParameters params { moves, false };
	(void) m_moveGenerator.GenerateMoves(params);

	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		bool draw = CheckForPotentialRepetition(depth - 1);
		m_board.UndoMove(move, undo);

		if (draw)
			return true;
	}

	return false;
}

Move Player::IterativeDeepening(int8_t maxDepth) {
	m_nodesSearched = 0;
	m_isStopped = false;

	// Initialise PV move to garbage move that will never clash with anything
	Move movePv{ QUIET_MOVE_BASE_SCORE, Square::NONE, Square::NONE };
	int16_t scorePv;
	int8_t depth = 1;
	m_killers.Reset();

	// Hack: Check if there is a chance we will threefold repeat. If so, clear TT table to make sure we don't use old value and repeat when winning.
	if (CheckForPotentialRepetition())
		m_transpositionTable.Clear();

	while (depth <= maxDepth) {
	
#if DEBUG
		m_currentDepthNodes = 0;
		m_quiescenceNodesSearched = 0;
#endif

		Move bestMove;
		scorePv = RootNegamax(depth, movePv, bestMove);

		if (m_isStopped)
			break;

		movePv = bestMove;

		++depth;

#if DEBUG
		std::cerr << "Log: Current depth nodes: " << m_currentDepthNodes << '\n';
		std::cerr << "Log: Current depth quiescence nodes searched: " << m_quiescenceNodesSearched << '\n';
		float ebf = pow(m_currentDepthNodes, (1.0f / depth));
		std::cerr << "Log: EBF: " << ebf << "\n\n"; 
#endif
	}

#if DEBUG
	std::cerr << "PV: " << movePv.ToString() << " ";
	Undo undo = m_board.MakeMove(movePv);
	PrintPv(depth - 2);
	m_board.UndoMove(movePv, undo);
#endif

	return movePv;
}

int16_t Player::RootNegamax(int8_t depth, const Move& movePv, Move& bestMove) {
#if DEBUG
	++m_currentDepthNodes;
	m_transpositionsHit = 0;
	std::cerr << "Log: Called root Negamax with depth " << (int) depth << '\n';
#endif

	++m_nodesSearched;

	if (m_board.CheckQuietDraws())
		return DRAW_SCORE;

	MoveList moves;
	MoveGenerationParameters params { moves, false };
	bool check = m_moveGenerator.GenerateMoves(params);

	if (moves.size() == 0) {
		return check ? -MATE_SCORE : DRAW_SCORE;
	}

	std::array<int, MoveList::MAX_POSSIBLE_MOVES> staticScores;
	for (int i = 0; i < moves.size(); ++i) {
		if (moves[i] == movePv)
			staticScores[i] = PV_MOVE_BASE_SCORE;
		else if (!moves[i].m_isCapture && moves[i] == m_killers.GetFirst(depth))
			staticScores[i] = FIRST_KILLER_BASE_SCORE;
		else if (!moves[i].m_isCapture && moves[i] == m_killers.GetSecond(depth))
			staticScores[i] = SECOND_KILLER_BASE_SCORE;
		else
			staticScores[i] = moves[i].m_score;
	}

	int16_t bestScore = -MAX_SCORE;
	int16_t alpha = -MAX_SCORE;
	int16_t beta = MAX_SCORE;

	for (int i = 0; i < moves.size(); ++i) {

		int best = i;
		for (int j = i + 1; j < moves.size(); ++j) {
			if (staticScores[j] > staticScores[best])
				best = j;
		}

		std::swap(moves[i], moves[best]);
		std::swap(staticScores[i], staticScores[best]);

		const Move& move = moves[i];

		Undo undo = m_board.MakeMove(move);
		int16_t score = -Negamax(depth - 1, -beta, -alpha);
		m_board.UndoMove(move, undo);

		if (m_isStopped)
			return NO_SCORE;

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
	std::cerr << "Log: Negamax " << (int) depth << " found the best move to be " << bestMove;
#endif

	return bestScore;
}

int16_t Player::Negamax(int8_t depth, int16_t alpha, int16_t beta) {
	++m_nodesSearched;
#if DEBUG
	++m_currentDepthNodes;
#endif

	if (m_isStopped)
		return NO_SCORE;

	if (((m_nodesSearched % 2048) == 0) && (Clock::now() >= m_deadline)) {
		m_isStopped = true;
		return NO_SCORE;
	}

	if (m_board.CheckQuietDraws())
		return DRAW_SCORE;

	Hash hash = m_board.GetHash();
	const TranspositionTableEntry* pEntry = m_transpositionTable.GetEntry(hash);

	bool isTransposition = pEntry != nullptr;

	if (isTransposition && (pEntry->m_depth >= depth)) {
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

	MoveGenerationContext context = m_moveGenerator.GetMoveGenerationContext();

	if (m_board.GetPhase() > NULL_MOVE_PHASE_LIMIT && !m_moveGenerator.IsCheck(context) && (depth > (NULL_MOVE_PRUNING_REDUCTION + 1))) {
		Undo undo = m_board.MakeNullMove();
		int16_t score = -Negamax(depth - NULL_MOVE_PRUNING_REDUCTION, -beta, -(beta - 1));
		m_board.UndoNullMove(undo);


		if (score >= beta) {
			return score;
		}
	}

	MoveList moves;
	MoveGenerationParameters params{ moves, false };
	bool check = m_moveGenerator.GenerateMoves(params, context);

	if (moves.size() == 0)
		return check ? -MATE_SCORE : DRAW_SCORE;

	if (depth == 0)
		return Quiescence(alpha, beta);

	std::array<int, MoveList::MAX_POSSIBLE_MOVES> staticScores;
	for (int i = 0; i < moves.size(); ++i) {
		if (isTransposition && (moves[i] == pEntry->m_move))
			staticScores[i] = TT_MOVE_BASE_SCORE;
		else if (!moves[i].m_isCapture && moves[i] == m_killers.GetFirst(depth))
			staticScores[i] = FIRST_KILLER_BASE_SCORE;
		else if (!moves[i].m_isCapture && moves[i] == m_killers.GetSecond(depth))
			staticScores[i] = SECOND_KILLER_BASE_SCORE;
		else
			staticScores[i] = moves[i].m_score;
	}

	int16_t bestScore = -MAX_SCORE;
	Move bestMove{ QUIET_MOVE_BASE_SCORE, Square::NONE, Square::NONE };
	EvaluationType evaluationType = EvaluationType::UPPER_BOUND;

	for (int i = 0; i < moves.size(); ++i) {

		int best = i;
		for (int j = i + 1; j < moves.size(); ++j) {
			if (staticScores[j] > staticScores[best])
				best = j;
		}

		std::swap(moves[i], moves[best]);
		std::swap(staticScores[i], staticScores[best]);

		const Move& move = moves[i];

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

		if (bestScore >= beta) {
			evaluationType = EvaluationType::LOWER_BOUND;

			if (!move.m_isCapture) {
				m_killers.Set(depth, move);
			}

			break;
		}
	}

	TranspositionTableEntry entry {
		bestMove,
		hash,
		bestScore,
		depth,
		evaluationType
	};

	m_transpositionTable.SetEntry(hash, entry);

	return bestScore;
}

int16_t Player::Quiescence(int16_t alpha, int16_t beta) {
	++m_nodesSearched;
#if DEBUG
	++m_quiescenceNodesSearched;
	++m_currentDepthNodes;
#endif

	if (m_isStopped)
		return 0;

	if (((m_nodesSearched % 2048) == 0) && (Clock::now() >= m_deadline)) {
		m_isStopped = true;
		return 0;
	}

	int8_t depth = 0;

	Hash hash = m_board.GetHash();
	const TranspositionTableEntry* pEntry = m_transpositionTable.GetEntry(hash);

	bool isTransposition = pEntry != nullptr;

	if (isTransposition && pEntry->m_depth == 0) {
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

	int16_t eval = Evaluate();

	if (eval >= beta) {
		TranspositionTableEntry entry {
			Move{ QUIET_MOVE_BASE_SCORE, Square::NONE, Square::NONE },
			hash,
			eval,
			depth,
			EvaluationType::LOWER_BOUND
		};

		m_transpositionTable.SetEntry(hash, entry);

		return eval;
	}

	if (eval > alpha)
		alpha = eval;

	MoveList captures;
	MoveGenerationParameters params{ captures, true };
	m_moveGenerator.GenerateMoves(params);

	std::array<int, MoveList::MAX_POSSIBLE_MOVES> staticScores;

	for (int i = 0; i < captures.size(); ++i) {
		if (isTransposition && (captures[i] == pEntry->m_move)) {
			staticScores[i] = TT_MOVE_BASE_SCORE;
		} else {
			staticScores[i] = captures[i].m_score;
		}
	}

	int16_t bestScore = eval;
	Move bestMove{ QUIET_MOVE_BASE_SCORE, Square::NONE, Square::NONE };
	EvaluationType evaluationType = EvaluationType::UPPER_BOUND;

	for (int i = 0; i < captures.size(); ++i) {
		int best = i;
		for (int j = i + 1; j < captures.size(); ++j) {
			if (staticScores[j] > staticScores[best])
				best = j;
		}

		std::swap(captures[i], captures[best]);
		std::swap(staticScores[i], staticScores[best]);

		const Move& capture = captures[i];

		int attackerValue = ABSOLUTE_PIECE_VALUES[m_board.GetPieceAtSquare(capture.m_from)];
		int victimValue = ABSOLUTE_PIECE_VALUES[m_board.GetPieceAtSquare(capture.m_to)];

		if ((capture.m_promotionPiece != Piece::EMPTY) && (attackerValue < victimValue))
			continue;

		int victimScore = ABSOLUTE_PIECE_VALUES[m_board.GetPieceAtSquare(capture.m_to)];
		if ((capture.m_promotionPiece != Piece::EMPTY) && ((eval + victimScore + DELTA_PRUNE_MARGIN) < alpha))
			continue;

		Undo undo = m_board.MakeMove(capture);
		int16_t score = -Quiescence(-beta, -alpha);
		m_board.UndoMove(capture, undo);

		if (score > bestScore) {
			bestScore = score;
			bestMove = capture;
		}

		if (score >= beta) {
			evaluationType = EvaluationType::UPPER_BOUND;
			break;
		}
		
		if (score > alpha) {
			alpha = score;
		}
	}

	TranspositionTableEntry entry {
		bestMove,
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

	MoveList moves;
	MoveGenerationParameters params{ moves, false };
	m_moveGenerator.GenerateMoves(params);

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
	MoveGenerationParameters params{ moves, false };
	m_moveGenerator.GenerateMoves(params);

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

void Player::PrintPv(int8_t depth) {
	if (depth == 0) {
		std::cerr << '\n';
		return;
	}

	Hash hash = m_board.GetHash();
	const TranspositionTableEntry* pEntry = m_transpositionTable.GetEntry(hash);

	if (pEntry == nullptr || pEntry->m_depth < 1) {
		std::cerr << "Log: PV ran out of moves...\n";
		return;
	}

	std::cerr << pEntry->m_move.ToString() << " ";

	Undo undo = m_board.MakeMove(pEntry->m_move);
	PrintPv(depth - 1);
	m_board.UndoMove(pEntry->m_move, undo);
}