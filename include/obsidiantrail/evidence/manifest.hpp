#ifndef OBSIDIANTRAIL_EVIDENCE_MANIFEST_HPP
#define OBSIDIANTRAIL_EVIDENCE_MANIFEST_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::evidence {

struct ManifestEntry {
    std::string name;
    std::string type;
    std::string digest;
    std::uint64_t size = 0;
    std::map<std::string, std::string> attributes;
};

struct Manifest {
    std::string case_id;
    std::string created_by;
    std::vector<ManifestEntry> entries;
    std::vector<std::string> warnings;
};

Manifest parse_manifest_text(const std::string& text);
std::string normalize_entry_name(const std::string& name);
std::vector<std::string> validate_manifest(const Manifest& manifest);
std::string summarize_manifest(const Manifest& manifest);

} // namespace obsidiantrail::evidence

#endif
