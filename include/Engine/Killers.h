#pragma once

#include "Engine/Constants.h"
#include "Engine/Move.h"


class Killers {
public:
	inline constexpr Killers() noexcept : m_killers{} { Reset(); }
	inline constexpr void Reset() noexcept { m_killers.fill(Move{ 0, Square::NONE, Square::NONE }); };

	inline const Move& GetFirst(int8_t depth) noexcept { return m_killers[FirstIndex(depth)]; }
	inline const Move& GetSecond(int8_t depth) noexcept { return m_killers[SecondIndex(depth)]; }

	inline void Set(int8_t depth, Move move) noexcept {
		if (move == m_killers[FirstIndex(depth)]) return;

		std::swap(m_killers[FirstIndex(depth)], m_killers[SecondIndex(depth)]);
		m_killers[FirstIndex(depth)] = move;
	}

private:
	inline size_t FirstIndex(int8_t depth) noexcept { return depth * 2; }
	inline size_t SecondIndex(int8_t depth) noexcept { return (depth * 2) + 1; }

	std::array<Move, MAX_DEPTH * 2> m_killers;
};