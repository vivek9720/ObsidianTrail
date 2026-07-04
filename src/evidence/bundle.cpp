#include "obsidiantrail/evidence/bundle.hpp"
#include "obsidiantrail/core/text.hpp"
#include <sstream>

namespace obsidiantrail::evidence {

Bundle parse_bundle_text(const std::string& text) {
    Bundle bundle;
    std::string manifest_text;
    std::string record_text;
    enum class Section { manifest, records } section = Section::manifest;
    for (const auto& line : core::lines(text)) {
        if (core::trim(line) == "[records]") { section = Section::records; continue; }
        if (core::trim(line) == "[manifest]") { section = Section::manifest; continue; }
        if (section == Section::manifest) manifest_text += line + "\n";
        else record_text += line + "\n";
    }
    bundle.manifest = parse_manifest_text(manifest_text);
    bundle.records = parse_record_text(record_text, bundle.manifest.case_id);
    bundle.ledger = ledger::build_ledger(bundle.records);
    return bundle;
}

std::string summarize_bundle(const Bundle& bundle) {
    std::ostringstream out;
    out << summarize_manifest(bundle.manifest) << "\n";
    out << ledger::summarize_ledger(bundle.ledger) << "\n";
    out << "records=" << bundle.records.size();
    return out.str();
}

} // namespace obsidiantrail::evidence
