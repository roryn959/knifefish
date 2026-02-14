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

	constexpr inline Bitboard& 	operator<<=(int s) 						noexcept	{ m_board <<= s; return *this; }
	constexpr inline Bitboard	operator<<(int s)				const	noexcept	{ Bitboard tmp = *this; return tmp <<= s; }

	constexpr inline Bitboard& 	operator>>=(int s) 						noexcept	{ m_board >>= s; return *this; }
	constexpr inline Bitboard	operator>>(int s)				const	noexcept	{ Bitboard tmp = *this; return tmp >>= s; }

	constexpr inline Bitboard&	operator|=(const Bitboard rhs)			noexcept 	{ m_board |= rhs.m_board; return *this; }
	constexpr inline Bitboard	operator|(const Bitboard rhs)	const	noexcept 	{ Bitboard tmp = *this; return tmp |= rhs; }

	constexpr inline Bitboard&	operator&=(const Bitboard rhs)			noexcept 	{ m_board &= rhs.m_board; return *this; }
	constexpr inline Bitboard	operator&(const Bitboard rhs)	const 	noexcept 	{ Bitboard tmp = *this; return tmp &= rhs; }

	constexpr inline Bitboard&	operator^=(const Bitboard rhs)			noexcept	{ m_board ^= rhs.m_board; return *this; }
	constexpr inline Bitboard	operator^(const Bitboard rhs)	const	noexcept	{ Bitboard tmp = *this; return tmp ^= rhs; }

	constexpr inline bool		operator==(const Bitboard rhs)	const	noexcept	{ return m_board == rhs.m_board; }
	constexpr inline bool		operator!=(const Bitboard rhs)	const 	noexcept	{ return m_board != rhs.m_board; }

	constexpr inline Bitboard	operator~()					const	noexcept	{ return Bitboard(~m_board); }

	Iterator begin() const { return Iterator{m_board}; }
	std::nullptr_t end() const { return nullptr; }


private:
	uint64_t m_board;
};

constexpr Bitboard A1_MASK { 0x0000000000000080ULL };
constexpr Bitboard B1_MASK { A1_MASK >> 1 };
constexpr Bitboard C1_MASK { A1_MASK >> 2 };
constexpr Bitboard D1_MASK { A1_MASK >> 3 };
constexpr Bitboard E1_MASK { A1_MASK >> 4 };
constexpr Bitboard F1_MASK { A1_MASK >> 5 };
constexpr Bitboard G1_MASK { A1_MASK >> 6 };
constexpr Bitboard H1_MASK { A1_MASK >> 7 };
constexpr Bitboard A2_MASK { 0x0000000000008000ULL };
constexpr Bitboard B2_MASK { A2_MASK >> 1 };
constexpr Bitboard C2_MASK { A2_MASK >> 2 };
constexpr Bitboard D2_MASK { A2_MASK >> 3 };
constexpr Bitboard E2_MASK { A2_MASK >> 4 };
constexpr Bitboard F2_MASK { A2_MASK >> 5 };
constexpr Bitboard G2_MASK { A2_MASK >> 6 };
constexpr Bitboard H2_MASK { A2_MASK >> 7 };

constexpr Bitboard A8_MASK { 0x8000000000000000ULL };
constexpr Bitboard B8_MASK { A8_MASK >> 1 };
constexpr Bitboard C8_MASK { A8_MASK >> 2 };
constexpr Bitboard D8_MASK { A8_MASK >> 3 };
constexpr Bitboard E8_MASK { A8_MASK >> 4 };
constexpr Bitboard F8_MASK { A8_MASK >> 5 };
constexpr Bitboard G8_MASK { A8_MASK >> 6 };
constexpr Bitboard H8_MASK { A8_MASK >> 7 };
constexpr Bitboard A7_MASK { 0x0080000000000000ULL };
constexpr Bitboard B7_MASK { A7_MASK >> 1 };
constexpr Bitboard C7_MASK { A7_MASK >> 2 };
constexpr Bitboard D7_MASK { A7_MASK >> 3 };
constexpr Bitboard E7_MASK { A7_MASK >> 4 };
constexpr Bitboard F7_MASK { A7_MASK >> 5 };
constexpr Bitboard G7_MASK { A7_MASK >> 6 };
constexpr Bitboard H7_MASK { A7_MASK >> 7 };

constexpr Bitboard RANK_2_MASK				{ 0x000000000000FF00ULL };
constexpr Bitboard RANK_7_MASK				{ 0x00FF000000000000ULL };

constexpr Bitboard NON_FILE_A_MASK			{ 0x7F7F7F7F7F7F7F7FULL };
constexpr Bitboard NON_FILE_A_OR_B_MASK		{ 0x3F3F3F3F3F3F3F3FULL };
constexpr Bitboard NON_FILE_G_OR_H_MASK		{ 0xFCFCFCFCFCFCFCFCULL };
constexpr Bitboard NON_FILE_H_MASK			{ 0xFEFEFEFEFEFEFEFEULL };

constexpr Bitboard WHITE_KINGSIDE_CASTLE_SPACE_MASK		{ F1_MASK | G1_MASK };
constexpr Bitboard WHITE_QUEENSIDE_CASTLE_SPACE_MASK	{ B1_MASK | C1_MASK | D1_MASK };
constexpr Bitboard BLACK_KINGSIDE_CASTLE_SPACE_MASK		{ F8_MASK | G8_MASK };
constexpr Bitboard BLACK_QUEENSIDE_CASTLE_SPACE_MASK	{ B8_MASK | C8_MASK | D8_MASK };