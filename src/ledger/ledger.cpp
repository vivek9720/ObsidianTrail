#include "obsidiantrail/ledger/ledger.hpp"
#include "obsidiantrail/core/text.hpp"
#include <sstream>

namespace obsidiantrail::ledger {

static std::uint64_t entry_checksum(std::uint64_t seq, std::uint64_t prev, const evidence::EvidenceRecord& record) {
    return core::stable_hash64(std::to_string(seq) + "|" + std::to_string(prev) + "|" + evidence::canonical_record_key(record) + "|" + record.raw);
}

Ledger build_ledger(const std::vector<evidence::EvidenceRecord>& records) {
    Ledger ledger;
    std::uint64_t previous = 0;
    std::uint64_t sequence = 1;
    for (const auto& record : records) {
        LedgerEntry entry;
        entry.sequence = sequence++;
        entry.previous_checksum = previous;
        entry.record = record;
        entry.checksum = entry_checksum(entry.sequence, entry.previous_checksum, entry.record);
        previous = entry.checksum;
        ledger.entries.push_back(std::move(entry));
    }
    return ledger;
}

Ledger parse_ledger_page(core::ByteView bytes) {
    Ledger ledger;
    core::Reader reader(bytes);
    auto magic = reader.bytes(4);
    if (!magic || magic.value().ascii_lossy() != "OTLG") {
        ledger.warnings.push_back("missing ledger magic");
        return ledger;
    }
    auto count = reader.varint();
    if (!count) {
        ledger.warnings.push_back("missing entry count");
        return ledger;
    }
    for (std::uint64_t i = 0; i < count.value() && reader.remaining() > 0 && i < 10000; ++i) {
        auto seq = reader.varint();
        auto prev = reader.be64();
        auto length = reader.varint();
        if (!seq || !prev || !length || length.value() > reader.remaining()) {
            ledger.warnings.push_back("truncated ledger entry");
            break;
        }
        auto payload = reader.bytes(static_cast<std::size_t>(length.value()));
        LedgerEntry entry;
        entry.sequence = seq.value();
        entry.previous_checksum = prev.value();
        entry.record = evidence::parse_record_line(payload.value().ascii_lossy(), static_cast<std::size_t>(i + 1));
        entry.checksum = entry_checksum(entry.sequence, entry.previous_checksum, entry.record);
        ledger.entries.push_back(std::move(entry));
    }
    if (!verify_ledger(ledger)) ledger.warnings.push_back("ledger chain does not verify");
    return ledger;
}

bool verify_ledger(const Ledger& ledger) {
    std::uint64_t previous = 0;
    std::uint64_t expected_sequence = 1;
    for (const auto& entry : ledger.entries) {
        if (entry.sequence != expected_sequence++) return false;
        if (entry.previous_checksum != previous) return false;
        auto computed = entry_checksum(entry.sequence, entry.previous_checksum, entry.record);
        if (computed != entry.checksum) return false;
        previous = entry.checksum;
    }
    return true;
}

std::vector<evidence::EvidenceRecord> replay(const Ledger& ledger) {
    std::vector<evidence::EvidenceRecord> records;
    for (const auto& entry : ledger.entries) records.push_back(entry.record);
    return records;
}

std::string summarize_ledger(const Ledger& ledger) {
    std::ostringstream out;
    out << "entries=" << ledger.entries.size() << " verified=" << (verify_ledger(ledger) ? "true" : "false");
    if (!ledger.entries.empty()) out << " head=" << core::hex64(ledger.entries.back().checksum);
    for (const auto& warning : ledger.warnings) out << "\nwarning: " << warning;
    return out.str();
}

} // namespace obsidiantrail::ledger
