#ifndef OBSIDIANTRAIL_ANALYSIS_CATALOGS_HPP
#define OBSIDIANTRAIL_ANALYSIS_CATALOGS_HPP

#include <string>
#include <vector>

namespace obsidiantrail::analysis {

struct EventSchema {
    std::string id;
    std::string platform;
    std::string provider;
    std::string family;
    std::string required_fields;
    std::string optional_fields;
    std::string parser_hint;
    std::string investigation_note;
};

struct ArtifactProfile {
    std::string extension;
    std::string family;
    std::string parser;
    std::string retention;
    std::string sensitivity;
    std::string analyst_note;
};

struct ResponseAction {
    std::string id;
    std::string phase;
    std::string action;
    std::string precondition;
    std::string risk;
    std::string evidence_needed;
};

const std::vector<EventSchema>& event_schemas();
const std::vector<ArtifactProfile>& artifact_profiles();
const std::vector<ResponseAction>& response_actions();
std::vector<EventSchema> schemas_for_platform(const std::string& platform);
std::vector<ArtifactProfile> profiles_for_family(const std::string& family);
std::vector<ResponseAction> actions_for_phase(const std::string& phase);
std::string catalog_summary();

} // namespace obsidiantrail::analysis

#endif
