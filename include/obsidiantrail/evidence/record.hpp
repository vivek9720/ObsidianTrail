#ifndef OBSIDIANTRAIL_EVIDENCE_RECORD_HPP
#define OBSIDIANTRAIL_EVIDENCE_RECORD_HPP

#include "obsidiantrail/core/time.hpp"
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::evidence {

enum class RecordKind { unknown, process, network, identity, file, alert, note, host, package };

struct EvidenceRecord {
    RecordKind kind = RecordKind::unknown;
    core::Timestamp timestamp;
    std::string source;
    std::string subject;
    std::string action;
    std::string object;
    std::string raw;
    std::map<std::string, std::string> fields;
    std::vector<std::string> tags;
};

std::string kind_name(RecordKind kind);
RecordKind parse_kind(const std::string& text);
EvidenceRecord parse_record_line(const std::string& line, std::size_t line_number = 0);
std::vector<EvidenceRecord> parse_record_text(const std::string& text, const std::string& source = {});
std::string canonical_record_key(const EvidenceRecord& record);
std::vector<std::string> validate_record(const EvidenceRecord& record);
std::string render_record(const EvidenceRecord& record);

} // namespace obsidiantrail::evidence

#endif
