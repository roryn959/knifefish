#pragma once

#include <cassert>

#include "Square.h"


class Bitboard {
public:

	class Iterator {
	public:
		uint64_t m_board;

		Square operator*() const {
			return Square(__builtin_ctzll(m_board));
		}

		Iterator& operator++() {
			m_board &= m_board - 1;
			return *this;
		}

		bool operator!=(std::nullptr_t) const {
			return m_board != 0;
		}
	};

	constexpr Bitboard() = default;
	constexpr Bitboard(uint64_t board) : m_board{board} {};

	constexpr inline uint64_t GetBoard() { return m_board; }

	constexpr inline bool IsEmpty() { return m_board == 0; }

	constexpr inline Bitboard PopLsb() {
		assert(m_board != 0);
	
		Bitboard lsb = m_board & -m_board;
		m_board &= m_board - 1;
		return lsb;
	}

	constexpr inline Bitboard& 	operator<<=(int s) 					noexcept	{ m_board <<= s; return *this; }
	constexpr inline Bitboard	operator<<(int s)			const	noexcept	{ Bitboard tmp = *this; tmp <<= s; return tmp; }

	constexpr inline Bitboard& 	operator>>=(int s) 					noexcept	{ m_board >>= s; return *this; }
	constexpr inline Bitboard	operator>>(int s)			const	noexcept	{ Bitboard tmp = *this; tmp >>= s; return tmp; }

	constexpr inline Bitboard&	operator|=(Bitboard rhs)			noexcept 	{ m_board |= rhs.m_board; return *this; }
	constexpr inline Bitboard	operator|(Bitboard rhs)		const	noexcept 	{ Bitboard tmp = *this; tmp |= rhs; return tmp; } // CHECK: Can we reduce this by one statement?

	constexpr inline Bitboard&	operator&=(Bitboard rhs)			noexcept 	{ m_board &= rhs.m_board; return *this; }
	constexpr inline Bitboard	operator&(Bitboard rhs)		const 	noexcept 	{ Bitboard tmp = *this; tmp &= rhs; return tmp; } // CHECK: Can we reduce this by one statement?

	constexpr inline Bitboard&	operator^=(Bitboard rhs)			noexcept	{ m_board ^= rhs.m_board; return *this; }
	constexpr inline Bitboard	operator^(Bitboard rhs)		const	noexcept	{ Bitboard tmp = *this; tmp ^= rhs; return tmp; } // CHECK: Can we reduce this by one statement?

	constexpr inline bool		operator==(Bitboard rhs)			noexcept	{ return m_board == rhs.m_board; }
	constexpr inline bool		operator!=(Bitboard rhs)	const 	noexcept	{ return m_board != rhs.m_board; }

	constexpr inline Bitboard	operator~()					const	noexcept	{ return Bitboard(~m_board); }

	Iterator begin() const { return Iterator{m_board}; }
	std::nullptr_t end() const { return nullptr; }


private:
	uint64_t m_board;
};

static_assert(sizeof(Bitboard) == sizeof(uint64_t));
