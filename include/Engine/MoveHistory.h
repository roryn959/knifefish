#pragma once

#include <array>

#include "BoardRepresentation/Square.h"
#include "Engine/Move.h"

#define MAX_HISTORY 7000


class MoveHistory {
public:
	MoveHistory();

	void Adjust(bool isWhiteTurn, const Move& move, int16_t amount) noexcept;
	int16_t Get(bool isWhiteTurn, const Move& move) const noexcept;

	void Decay() noexcept;

private:
	std::array<std::array<std::array<int16_t, static_cast<size_t>(Square::COUNT)>, static_cast<size_t>(Square::COUNT)>, 2> m_history;
};

inline MoveHistory::MoveHistory() :
	m_history{}
{
	for (size_t i=0; i < static_cast<size_t>(Square::COUNT); ++i) {
		for (size_t j=0; j < static_cast<size_t>(Square::COUNT); ++j) {
			m_history[0][i][j] = 0;
			m_history[1][i][j] = 0;
		}
	}
}

inline void MoveHistory::Adjust(bool isWhiteTurn, const Move& move, int16_t amount) noexcept {
	int16_t& score = m_history[isWhiteTurn ? 0 : 1][static_cast<size_t>(move.m_from)][static_cast<size_t>(move.m_to)];
	score += amount;
	if (score >= MAX_HISTORY or score <= -MAX_HISTORY)
		Decay();
}

inline int16_t MoveHistory::Get(bool isWhiteTurn, const Move& move) const noexcept {
	return m_history[isWhiteTurn ? 0 : 1][static_cast<size_t>(move.m_from)][static_cast<size_t>(move.m_to)];
}

inline void MoveHistory::Decay() noexcept {
	for (size_t i=0; i < static_cast<size_t>(Square::COUNT); ++i) {
		for (size_t j=0; j < static_cast<size_t>(Square::COUNT); ++j) {
			m_history[0][i][j] /= 2;
			m_history[1][i][j] /= 2;
		}
	}
}