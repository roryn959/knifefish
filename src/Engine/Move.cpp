#include "Engine/Move.h"

std::ostream& operator<<(std::ostream& os, Move move) {
	os <<
	GetSquareFromBitmask(move.m_from.GetBoard()) <<
	GetSquareFromBitmask(move.m_to.GetBoard()) <<
	' ' <<
	GetChar(move.m_promotionPiece) <<
	' ' <<
	move.m_isCapture <<
	' ' <<
	move.m_isDoublePawnPush <<
	' ' <<
	move.m_isEnPassant <<
	' ' <<
	move.m_isCastle <<
	'\n';
	return os;
}