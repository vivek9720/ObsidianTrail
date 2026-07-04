#ifndef OBSIDIANTRAIL_ANALYSIS_INVESTIGATION_ATLAS_HPP
#define OBSIDIANTRAIL_ANALYSIS_INVESTIGATION_ATLAS_HPP

#include <string>
#include <vector>

namespace obsidiantrail::analysis {

struct InvestigationTemplate {
    std::string id;
    std::string scenario;
    std::string objective;
    std::string query;
    std::string pivot;
    std::string evidence;
    std::string review;
};

const std::vector<InvestigationTemplate>& investigation_atlas();
std::vector<InvestigationTemplate> templates_for_scenario(const std::string& scenario);
std::vector<InvestigationTemplate> templates_for_evidence(const std::string& evidence);
std::string investigation_atlas_summary();

} // namespace obsidiantrail::analysis

#endif
