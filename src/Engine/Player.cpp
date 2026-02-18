#include "Engine/Player.h"

//./cutechess-cli -engine cmd=./engines/basic_negamax name=Player1 proto=uci stderr=player1_logs -engine cmd=./engines/basic_negamax name=Player2 proto=uci stderr=player2_logs -each tc=inf depth=4 -games 1 -pgnout games.pgn


Player::Player(Board& board) :
	m_board{board},
	m_moveGenerator{m_board}
#if DEBUG
	,
	m_nodesSearched{0}
#endif
{}

Move Player::GoDepth(int depth) {
#if DEBUG
	auto startTime = std::chrono::system_clock::now();
#endif

	Move bestMove = RootNegamax(depth);

#if DEBUG
	auto endTime = std::chrono::system_clock::now();
	auto searchTime = endTime - startTime;
	auto searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(searchTime).count();
	auto searchTimeS = searchTimeMs / 1000.0;

	std::cerr << "Log: Nodes searched: " << m_nodesSearched << '\n';
	std::cerr << "Log: Search time (s): " << searchTimeS << '\n';
	std::cerr << "Log: Search speed (nps): " << m_nodesSearched / searchTimeS << '\n';
#endif

	return bestMove;
}

int Player::Evaluate() {
	int eval = 0;

	Bitboard piecePositions;
	#define X(piece) piecePositions = m_board.GetPieceBitboard(Piece::piece); while (piecePositions.Any()) { eval += PIECE_VALUES[Piece::piece]; piecePositions.PopLsb(); }
	PIECES_LIST
	#undef X

	return eval;
}

Move Player::RootNegamax(int depth) {
#if DEBUG
	m_nodesSearched = 1;
#endif

	int colour = m_board.IsWhiteTurn() ? 1 : -1;
	int bestScore = INT_MIN;
	Move bestMove;
	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();	

	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		int score = -Negamax(depth - 1, -colour);
		if (score > bestScore) {
			bestScore = score;
			bestMove = move;
		}
		m_board.UndoMove(move, undo);
	}

	std::cerr << "Log: Negamax return an evaluation of " << bestScore << "\n";
	return bestMove;
}

int Player::Negamax(int depth, int colour) {
#if DEBUG
	++m_nodesSearched;
#endif

	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();
	if (moves.size() == 0) {
		if (colour == 1) {
			if (m_board.IsAttackedByBlack(m_board.GetPieceBitboard(Piece::WHITE_KING))) 
				return INT_MIN;
			return 0;
		} else {
			if (m_board.IsAttackedByWhite(m_board.GetPieceBitboard(Piece::BLACK_KING))) 
				return INT_MAX;
			return 0;
		}
	}

	if (depth == 0)
		return colour * Evaluate();

	int bestScore = INT_MIN;

	for (const Move& move : moves) {
		Undo undo = m_board.MakeMove(move);
		int score = -Negamax(depth - 1, -colour);
		if (score > bestScore)
			bestScore = score;
		m_board.UndoMove(move, undo);
	}
	return bestScore;
}