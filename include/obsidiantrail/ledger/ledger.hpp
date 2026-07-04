#ifndef OBSIDIANTRAIL_LEDGER_LEDGER_HPP
#define OBSIDIANTRAIL_LEDGER_LEDGER_HPP

#include "obsidiantrail/core/bytes.hpp"
#include "obsidiantrail/evidence/record.hpp"
#include <string>
#include <vector>

namespace obsidiantrail::ledger {

struct LedgerEntry {
    std::uint64_t sequence = 0;
    std::uint64_t previous_checksum = 0;
    std::uint64_t checksum = 0;
    evidence::EvidenceRecord record;
};

struct Ledger {
    std::vector<LedgerEntry> entries;
    std::vector<std::string> warnings;
};

Ledger build_ledger(const std::vector<evidence::EvidenceRecord>& records);
Ledger parse_ledger_page(core::ByteView bytes);
bool verify_ledger(const Ledger& ledger);
std::string summarize_ledger(const Ledger& ledger);
std::vector<evidence::EvidenceRecord> replay(const Ledger& ledger);

} // namespace obsidiantrail::ledger

#endif
