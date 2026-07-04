#include "obsidiantrail/evidence/manifest.hpp"
#include "obsidiantrail/core/text.hpp"
#include <cstdlib>
#include <set>
#include <sstream>

namespace obsidiantrail::evidence {

std::string normalize_entry_name(const std::string& name) {
    auto decoded = core::percent_decode_lossy(name);
    std::vector<std::string> kept;
    for (auto part : core::split(decoded, '/')) {
        part = core::trim(part);
        if (part.empty() || part == ".") continue;
        if (part == "..") {
            if (!kept.empty()) kept.pop_back();
            continue;
        }
        kept.push_back(part);
    }
    return core::join(kept, "/");
}

Manifest parse_manifest_text(const std::string& text) {
    Manifest manifest;
    ManifestEntry current;
    bool have_entry = false;
    auto flush = [&]() {
        if (have_entry) {
            current.name = normalize_entry_name(current.name);
            manifest.entries.push_back(current);
            current = ManifestEntry{};
            have_entry = false;
        }
    };
    for (auto line : core::lines(text)) {
        line = core::trim(line);
        if (line.empty() || core::starts_with(line, "#")) continue;
        if (line == "---") { flush(); continue; }
        auto pairs = core::parse_pairs(line);
        if (pairs.count("case")) manifest.case_id = pairs["case"];
        if (pairs.count("case_id")) manifest.case_id = pairs["case_id"];
        if (pairs.count("created_by")) manifest.created_by = pairs["created_by"];
        if (pairs.count("entry") || pairs.count("name")) {
            flush();
            have_entry = true;
            current.name = pairs.count("entry") ? pairs["entry"] : pairs["name"];
        }
        if (pairs.count("type")) { have_entry = true; current.type = pairs["type"]; }
        if (pairs.count("digest")) { have_entry = true; current.digest = pairs["digest"]; }
        if (pairs.count("sha256")) { have_entry = true; current.digest = pairs["sha256"]; }
        if (pairs.count("size")) { have_entry = true; current.size = std::strtoull(pairs["size"].c_str(), nullptr, 10); }
        for (const auto& kv : pairs) current.attributes[kv.first] = kv.second;
    }
    flush();
    manifest.warnings = validate_manifest(manifest);
    return manifest;
}

std::vector<std::string> validate_manifest(const Manifest& manifest) {
    std::vector<std::string> warnings;
    std::set<std::string> names;
    if (manifest.case_id.empty()) warnings.push_back("manifest missing case id");
    for (const auto& entry : manifest.entries) {
        if (entry.name.empty()) warnings.push_back("entry with empty name");
        if (!names.insert(entry.name).second) warnings.push_back("duplicate entry " + entry.name);
        if (entry.digest.empty()) warnings.push_back("entry missing digest " + entry.name);
        if (entry.size == 0 && entry.type != "note") warnings.push_back("zero-sized non-note entry " + entry.name);
        if (core::starts_with(entry.name, "/")) warnings.push_back("absolute entry path " + entry.name);
        if (entry.name.find("..") != std::string::npos) warnings.push_back("suspicious entry path " + entry.name);
    }
    return warnings;
}

std::string summarize_manifest(const Manifest& manifest) {
    std::uint64_t total = 0;
    std::map<std::string, std::size_t> by_type;
    for (const auto& entry : manifest.entries) {
        total += entry.size;
        by_type[entry.type]++;
    }
    std::ostringstream out;
    out << "case=" << manifest.case_id << " entries=" << manifest.entries.size() << " bytes=" << total;
    for (const auto& kv : by_type) out << " " << kv.first << "=" << kv.second;
    for (const auto& warning : manifest.warnings) out << "\nwarning: " << warning;
    return out.str();
}

} // namespace obsidiantrail::evidence
