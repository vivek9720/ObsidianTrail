#ifndef OBSIDIANTRAIL_EVIDENCE_PATH_HPP
#define OBSIDIANTRAIL_EVIDENCE_PATH_HPP

#include <string>
#include <vector>

namespace obsidiantrail::evidence {

struct ArtifactPath {
    std::string original;
    std::string normalized;
    bool absolute = false;
    bool traversal = false;
    bool windows = false;
    std::vector<std::string> segments;
};

ArtifactPath parse_artifact_path(const std::string& text);
std::string join_path(const std::vector<std::string>& segments);
std::vector<std::string> path_warnings(const ArtifactPath& path);
std::string classify_path(const ArtifactPath& path);

} // namespace obsidiantrail::evidence

#endif
