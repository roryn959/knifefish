#pragma once

#include <array>

#include "Engine/Constants.h"
#include "Engine/Move.h"


class PrincipleVariation {
public:
	PrincipleVariation() noexcept;

	void Set(size_t ply, Move move) noexcept;
	Move Get(size_t ply) const noexcept;

	size_t GetLength(size_t ply=0) const noexcept;

	void Reset() noexcept;
	void Reset(size_t ply) noexcept;

private:
	std::array<std::array<Move, MAX_DEPTH>, MAX_DEPTH> m_line;
	std::array<size_t, MAX_DEPTH> m_length;
};

inline PrincipleVariation::PrincipleVariation() noexcept {
	Reset();
}

inline void PrincipleVariation::Set(size_t ply, Move move) noexcept {
	m_line[ply][0] = move;

	for (size_t i = 0; i < m_length[ply+1]; ++i) {
#if DEBUG
		if (m_line[ply+1][i].m_from == Square::NONE) {
			std::cerr << "PV invalid move error; next ply length: " << m_length[ply+1] << ", trying to get pv move of ply: " << ply+1 << '\n';
			std::abort();
		}
#endif
		m_line[ply][i+1] = m_line[ply+1][i];
	}

	m_length[ply] = m_length[ply+1] + 1;
}

inline Move PrincipleVariation::Get(size_t ply) const noexcept {
#if DEBUG
	if (m_length[ply] < 0) {
		std::cerr << "PV invalid length error; trying to get pv of ply: " << ply << ", pv length of root: " << m_length[0] << '\n';
		std::abort();
	}
#endif

	return m_line[ply][0];
}

inline size_t PrincipleVariation::GetLength(size_t ply) const noexcept {
	return m_length[ply];
}

inline void PrincipleVariation::Reset() noexcept {
	m_length.fill(0);
}

inline void PrincipleVariation::Reset(size_t ply) noexcept {
	m_length[ply] = 0;
}

inline std::ostream& operator<<(std::ostream& os, PrincipleVariation pv) {
	os << "PV: ";

	for (size_t i=0; i < pv.GetLength(); ++i)
		os << pv.Get(i).ToString() << ", ";

	os << "\n\n";

	return os;
}
