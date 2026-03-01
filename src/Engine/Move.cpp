#include "Engine/Move.h"


std::string Move::ToString() const {
	std::string string1 = SquareToString(m_from);
	std::string string2 = SquareToString(m_to);
	std::string string3 = "";
	if (m_promotionPiece != Piece::EMPTY) {
		char cPiece = GetChar(m_promotionPiece);
		string3 += std::tolower(cPiece);
	}
	return string1 + string2 + string3;
}

std::ostream& operator<<(std::ostream& os, Move move) {
	os <<
	move.m_from <<
	move.m_to <<
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