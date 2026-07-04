#include "obsidiantrail/evidence/path.hpp"
#include "obsidiantrail/core/text.hpp"
#include <sstream>

namespace obsidiantrail::evidence {

std::string join_path(const std::vector<std::string>& segments) {
    return core::join(segments, "/");
}

ArtifactPath parse_artifact_path(const std::string& text) {
    ArtifactPath path;
    path.original = text;
    std::string decoded = core::percent_decode_lossy(text);
    path.windows = decoded.find('\\') != std::string::npos || (decoded.size() > 2 && decoded[1] == ':');
    for (char& ch : decoded) if (ch == '\\') ch = '/';
    path.absolute = core::starts_with(decoded, "/") || (decoded.size() > 2 && decoded[1] == ':');
    for (auto part : core::split(decoded, '/')) {
        part = core::trim(part);
        if (part.empty() || part == ".") continue;
        if (part == "..") {
            path.traversal = true;
            if (!path.segments.empty()) path.segments.pop_back();
            continue;
        }
        path.segments.push_back(part);
    }
    path.normalized = join_path(path.segments);
    return path;
}

std::vector<std::string> path_warnings(const ArtifactPath& path) {
    std::vector<std::string> warnings;
    if (path.absolute) warnings.push_back("absolute artifact path");
    if (path.traversal) warnings.push_back("path contains traversal");
    if (path.normalized.empty()) warnings.push_back("empty normalized path");
    if (path.normalized.find(':') != std::string::npos) warnings.push_back("path contains stream or drive separator");
    if (path.segments.size() > 24) warnings.push_back("deep artifact path");
    for (const auto& segment : path.segments) {
        if (segment.size() > 180) warnings.push_back("long path segment");
        if (core::contains_folded(segment, "password") || core::contains_folded(segment, "secret")) warnings.push_back("sensitive path label");
    }
    return warnings;
}

std::string classify_path(const ArtifactPath& path) {
    auto p = core::lower(path.normalized);
    if (core::ends_with(p, ".evtx") || core::ends_with(p, ".etl")) return "windows-log";
    if (core::ends_with(p, ".pcap") || core::ends_with(p, ".pcapng")) return "packet-capture";
    if (core::ends_with(p, ".json") || core::ends_with(p, ".jsonl")) return "structured-log";
    if (core::ends_with(p, ".zip") || core::ends_with(p, ".tar")) return "archive";
    if (p.find("/registry/") != std::string::npos) return "registry";
    if (p.find("/memory/") != std::string::npos) return "memory";
    return "artifact";
}

} // namespace obsidiantrail::evidence
