#pragma once

#include <cassert>

#include "Square.h"


constexpr uint64_t A1_MASK { 0x0000000000000080ULL };
constexpr uint64_t B1_MASK { A1_MASK >> 1 };
constexpr uint64_t C1_MASK { A1_MASK >> 2 };
constexpr uint64_t D1_MASK { A1_MASK >> 3 };
constexpr uint64_t E1_MASK { A1_MASK >> 4 };
constexpr uint64_t F1_MASK { A1_MASK >> 5 };
constexpr uint64_t G1_MASK { A1_MASK >> 6 };
constexpr uint64_t H1_MASK { A1_MASK >> 7 };
constexpr uint64_t A2_MASK { 0x0000000000008000ULL };
constexpr uint64_t B2_MASK { A2_MASK >> 1 };
constexpr uint64_t C2_MASK { A2_MASK >> 2 };
constexpr uint64_t D2_MASK { A2_MASK >> 3 };
constexpr uint64_t E2_MASK { A2_MASK >> 4 };
constexpr uint64_t F2_MASK { A2_MASK >> 5 };
constexpr uint64_t G2_MASK { A2_MASK >> 6 };
constexpr uint64_t H2_MASK { A2_MASK >> 7 };

constexpr uint64_t A8_MASK { 0x8000000000000000ULL };
constexpr uint64_t B8_MASK { A8_MASK >> 1 };
constexpr uint64_t C8_MASK { A8_MASK >> 2 };
constexpr uint64_t D8_MASK { A8_MASK >> 3 };
constexpr uint64_t E8_MASK { A8_MASK >> 4 };
constexpr uint64_t F8_MASK { A8_MASK >> 5 };
constexpr uint64_t G8_MASK { A8_MASK >> 6 };
constexpr uint64_t H8_MASK { A8_MASK >> 7 };
constexpr uint64_t A7_MASK { 0x0080000000000000ULL };
constexpr uint64_t B7_MASK { A7_MASK >> 1 };
constexpr uint64_t C7_MASK { A7_MASK >> 2 };
constexpr uint64_t D7_MASK { A7_MASK >> 3 };
constexpr uint64_t E7_MASK { A7_MASK >> 4 };
constexpr uint64_t F7_MASK { A7_MASK >> 5 };
constexpr uint64_t G7_MASK { A7_MASK >> 6 };
constexpr uint64_t H7_MASK { A7_MASK >> 7 };

constexpr uint64_t RANK_2_MASK				{ 0x000000000000FF00ULL };
constexpr uint64_t RANK_7_MASK				{ 0x00FF000000000000ULL };

constexpr uint64_t NON_FILE_A_MASK			{ 0x7F7F7F7F7F7F7F7FULL };
constexpr uint64_t NON_FILE_A_OR_B_MASK		{ 0x3F3F3F3F3F3F3F3FULL };
constexpr uint64_t NON_FILE_G_OR_H_MASK		{ 0xFCFCFCFCFCFCFCFCULL };
constexpr uint64_t NON_FILE_H_MASK			{ 0xFEFEFEFEFEFEFEFEULL };

constexpr uint64_t WHITE_KINGSIDE_CASTLE_SPACE_MASK		{ F1_MASK | G1_MASK };
constexpr uint64_t WHITE_QUEENSIDE_CASTLE_SPACE_MASK	{ B1_MASK | C1_MASK | D1_MASK };
constexpr uint64_t BLACK_KINGSIDE_CASTLE_SPACE_MASK		{ F8_MASK | G8_MASK };
constexpr uint64_t BLACK_QUEENSIDE_CASTLE_SPACE_MASK	{ B8_MASK | C8_MASK | D8_MASK };


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
	constexpr Bitboard(uint64_t board) noexcept : m_board{board} {};

	constexpr inline uint64_t GetBoard() { return m_board; }

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

	constexpr inline Bitboard	operator~()						const	noexcept	{ return Bitboard(~m_board); }

	constexpr explicit operator bool() const noexcept { return static_cast<bool>(m_board); }

	// Note: We should not need rank masks here because shifting should throw away invalid vertical moves anyway.
	constexpr Bitboard ShiftNorth() 	const noexcept { return m_board << 8; }
	constexpr Bitboard ShiftEast() 		const noexcept { return (m_board & NON_FILE_H_MASK) >> 1; }
	constexpr Bitboard ShiftSouth() 	const noexcept { return m_board >> 8; }
	constexpr Bitboard ShiftWest() 		const noexcept { return (m_board & NON_FILE_A_MASK) << 1; }
	constexpr Bitboard ShiftNorthEast() const noexcept { return (m_board & NON_FILE_H_MASK) << 7; }
	constexpr Bitboard ShiftSouthEast() const noexcept { return (m_board & NON_FILE_H_MASK) >> 9; }
	constexpr Bitboard ShiftSouthWest() const noexcept { return (m_board & NON_FILE_A_MASK) >> 7; }
	constexpr Bitboard ShiftNorthWest() const noexcept { return (m_board & NON_FILE_A_MASK) << 9; }

	Iterator begin() const { return Iterator{m_board}; }
	std::nullptr_t end() const { return nullptr; }


private:
	uint64_t m_board;
};