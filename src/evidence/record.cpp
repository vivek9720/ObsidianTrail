#include "obsidiantrail/evidence/record.hpp"
#include "obsidiantrail/core/text.hpp"
#include <sstream>

namespace obsidiantrail::evidence {

std::string kind_name(RecordKind kind) {
    switch (kind) {
        case RecordKind::process: return "process";
        case RecordKind::network: return "network";
        case RecordKind::identity: return "identity";
        case RecordKind::file: return "file";
        case RecordKind::alert: return "alert";
        case RecordKind::note: return "note";
        case RecordKind::host: return "host";
        case RecordKind::package: return "package";
        default: return "unknown";
    }
}

RecordKind parse_kind(const std::string& text) {
    auto k = core::lower(core::trim(text));
    if (k == "proc" || k == "process" || k == "exec") return RecordKind::process;
    if (k == "net" || k == "network" || k == "flow" || k == "dns") return RecordKind::network;
    if (k == "id" || k == "identity" || k == "login" || k == "session") return RecordKind::identity;
    if (k == "file" || k == "artifact" || k == "path") return RecordKind::file;
    if (k == "alert" || k == "finding" || k == "signal") return RecordKind::alert;
    if (k == "note" || k == "comment") return RecordKind::note;
    if (k == "host" || k == "asset") return RecordKind::host;
    if (k == "package" || k == "manifest") return RecordKind::package;
    return RecordKind::unknown;
}

EvidenceRecord parse_record_line(const std::string& line, std::size_t line_number) {
    EvidenceRecord record;
    record.raw = line;
    record.fields = core::parse_pairs(line);
    auto field = [&](const std::string& key) -> std::string {
        auto it = record.fields.find(core::normalize_key(key));
        return it == record.fields.end() ? std::string() : it->second;
    };
    record.kind = parse_kind(field("kind").empty() ? field("type") : field("kind"));
    record.source = field("source");
    if (record.source.empty()) record.source = field("sensor");
    record.subject = field("subject");
    if (record.subject.empty()) record.subject = field("user");
    if (record.subject.empty()) record.subject = field("principal");
    record.action = field("action");
    if (record.action.empty()) record.action = field("event");
    record.object = field("object");
    if (record.object.empty()) record.object = field("target");
    if (record.object.empty()) record.object = field("path");
    auto ts = field("time");
    if (ts.empty()) ts = field("ts");
    if (!ts.empty()) {
        auto unix = core::parse_unix_time(ts);
        record.timestamp = unix ? unix.value() : core::parse_iso8601(ts).value();
    } else {
        record.timestamp = core::normalize_time(static_cast<std::int64_t>(line_number), 0, "line");
    }
    for (const auto& part : core::split(field("tags"), ',')) {
        auto tag = core::trim(part);
        if (!tag.empty()) record.tags.push_back(tag);
    }
    return record;
}

std::vector<EvidenceRecord> parse_record_text(const std::string& text, const std::string& source) {
    std::vector<EvidenceRecord> records;
    std::size_t line_number = 1;
    for (const auto& line : core::lines(text)) {
        auto clean = core::trim(line);
        if (clean.empty() || core::starts_with(clean, "#")) { ++line_number; continue; }
        auto record = parse_record_line(clean, line_number++);
        if (record.source.empty()) record.source = source;
        records.push_back(std::move(record));
    }
    return records;
}

std::string canonical_record_key(const EvidenceRecord& record) {
    return kind_name(record.kind) + "|" + record.timestamp.to_string() + "|" +
           core::lower(record.source) + "|" + core::lower(record.subject) + "|" +
           core::lower(record.action) + "|" + core::lower(record.object);
}

std::vector<std::string> validate_record(const EvidenceRecord& record) {
    std::vector<std::string> warnings;
    if (record.kind == RecordKind::unknown) warnings.push_back("unknown record kind");
    if (record.source.empty()) warnings.push_back("missing source");
    if (record.action.empty() && record.kind != RecordKind::note) warnings.push_back("missing action");
    if (record.subject.empty() && (record.kind == RecordKind::identity || record.kind == RecordKind::process)) warnings.push_back("missing subject");
    if (record.object.empty() && (record.kind == RecordKind::network || record.kind == RecordKind::file)) warnings.push_back("missing object");
    if (record.raw.size() > 16384) warnings.push_back("large single-line record");
    return warnings;
}

std::string render_record(const EvidenceRecord& record) {
    std::ostringstream out;
    out << record.timestamp.to_string() << " " << kind_name(record.kind);
    if (!record.source.empty()) out << " source=" << record.source;
    if (!record.subject.empty()) out << " subject=" << record.subject;
    if (!record.action.empty()) out << " action=" << record.action;
    if (!record.object.empty()) out << " object=" << record.object;
    if (!record.tags.empty()) out << " tags=" << core::join(record.tags, ",");
    return out.str();
}

} // namespace obsidiantrail::evidence
