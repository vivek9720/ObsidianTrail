#ifndef OBSIDIANTRAIL_EVIDENCE_BUNDLE_HPP
#define OBSIDIANTRAIL_EVIDENCE_BUNDLE_HPP

#include "obsidiantrail/evidence/manifest.hpp"
#include "obsidiantrail/evidence/record.hpp"
#include "obsidiantrail/ledger/ledger.hpp"
#include <string>

namespace obsidiantrail::evidence {

struct Bundle {
    Manifest manifest;
    std::vector<EvidenceRecord> records;
    ledger::Ledger ledger;
};

Bundle parse_bundle_text(const std::string& text);
std::string summarize_bundle(const Bundle& bundle);

} // namespace obsidiantrail::evidence

#endif
