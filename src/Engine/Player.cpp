#include "Engine/Player.h"

Player::Player(Board& board) :
	m_board{board},
	m_moveGenerator{m_board}
{
	srand(time(0));
}

Move Player::GetMove() {
	std::vector<Move> moves = m_moveGenerator.GenerateLegalMoves();

	for (const Move& m : moves) {
		std::cout << m;
	}

	int moveIndex = rand() % moves.size();

	Move move = moves[moveIndex];

	std::cout << "Chosen move: \n" << move;

	return move;
}