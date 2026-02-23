#pragma once

#include <vector>

#include "Config.h"
#include "BoardRepresentation/Zobrist.h"
#include "Engine/Move.h"


enum class EvaluationType : uint8_t {
	EXACT,
	LOWER_BOUND,
	UPPER_BOUND
};

struct TranspositionTableEntry {
	Hash 			m_key;
	int16_t 		m_score;
	int8_t 			m_depth;
	EvaluationType 	m_evaluationType;
};

constexpr size_t TRANSPOSITION_TABLE_SIZE_BYTES = TRANSPOSITION_TABLE_SIZE_MB * 1024 * 1024;
constexpr size_t TRANSPOSITION_TABLE_RAW_NUM_ENTRIES = TRANSPOSITION_TABLE_SIZE_BYTES / sizeof(TranspositionTableEntry);

class TranspositionTable {
public:
	TranspositionTable();

	const TranspositionTableEntry* GetEntry(Hash key) const;

	void SetEntry(Hash key, TranspositionTableEntry entry);

private:
	size_t m_numEntries;
	std::vector<TranspositionTableEntry> m_table;

};