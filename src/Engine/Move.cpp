#include "Engine/Move.h"


std::string Move::ToString() const {
	std::string string1 = SquareToString(GetSquareFromBitmask(m_from.GetBoard()));
	std::string string2 = SquareToString(GetSquareFromBitmask(m_to.GetBoard()));
	return string1 + string2;
}

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