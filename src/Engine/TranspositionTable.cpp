#include "Engine/TranspositionTable.h"


TranspositionTable::TranspositionTable() :
	m_numEntries{ 1ULL << static_cast<int>(floor(log2(TRANSPOSITION_TABLE_RAW_NUM_ENTRIES))) },
	m_table{}
{
	std::cerr << "Log: Creating transposition table with " << m_numEntries << " entries.\n";

	m_table.resize(m_numEntries);
}

void TranspositionTable::Clear() {
	for (size_t i = 0; i < m_numEntries; ++i) {
		m_table[i] = TranspositionTableEntry{ Move{ 0ULL, Square::NONE, Square::NONE }, 0ULL, 0, -1 };
	}
}

const TranspositionTableEntry* TranspositionTable::GetEntry(Hash key) const {
	size_t index = key & (m_numEntries - 1);

	const TranspositionTableEntry& entry = m_table[index];

	if (entry.m_key == 0)
		return nullptr;

	if (key == entry.m_key)
		return &entry;
	
	return nullptr;
}

void TranspositionTable::SetEntry(Hash key, TranspositionTableEntry entry) {
	size_t index = key & (m_numEntries - 1);
	m_table[index] = entry;
}