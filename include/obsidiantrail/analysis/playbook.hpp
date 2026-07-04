#ifndef OBSIDIANTRAIL_ANALYSIS_PLAYBOOK_HPP
#define OBSIDIANTRAIL_ANALYSIS_PLAYBOOK_HPP

#include "obsidiantrail/timeline/timeline.hpp"
#include <string>
#include <vector>

namespace obsidiantrail::analysis {

struct PlaybookRule {
    std::string id;
    std::string tactic;
    std::string technique;
    std::string field;
    std::string needle;
    int severity = 0;
    std::string guidance;
};

const std::vector<PlaybookRule>& built_in_playbook();
std::vector<PlaybookRule> rules_for_tactic(const std::string& tactic);
std::vector<std::string> apply_playbook(const timeline::Timeline& timeline);
std::string summarize_playbook();

} // namespace obsidiantrail::analysis

#endif
