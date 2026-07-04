#ifndef OBSIDIANTRAIL_TIMELINE_TIMELINE_HPP
#define OBSIDIANTRAIL_TIMELINE_TIMELINE_HPP

#include "obsidiantrail/evidence/record.hpp"
#include "obsidiantrail/identity/identity.hpp"
#include "obsidiantrail/network/network.hpp"
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::timeline {

struct TimelineEvent {
    evidence::EvidenceRecord record;
    int risk = 0;
    std::vector<std::string> reasons;
};

struct Timeline {
    std::vector<TimelineEvent> events;
    std::map<std::string, int> risk_by_subject;
    std::vector<std::string> findings;
};

Timeline build_timeline(const std::vector<evidence::EvidenceRecord>& records);
std::vector<TimelineEvent> window(const Timeline& timeline, core::Timestamp start, core::Timestamp end);
std::vector<std::string> correlate(const Timeline& timeline);
std::string summarize_timeline(const Timeline& timeline);

} // namespace obsidiantrail::timeline

#endif
