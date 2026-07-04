#ifndef OBSIDIANTRAIL_ANALYSIS_STATISTICS_HPP
#define OBSIDIANTRAIL_ANALYSIS_STATISTICS_HPP

#include "obsidiantrail/timeline/timeline.hpp"
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::analysis {

struct NumericSummary {
    double min = 0;
    double max = 0;
    double mean = 0;
    double median = 0;
    double p95 = 0;
};

struct TimelineStatistics {
    std::map<std::string, std::size_t> events_by_kind;
    std::map<std::string, std::size_t> events_by_source;
    std::map<std::string, std::size_t> actions_by_subject;
    NumericSummary risk;
    std::vector<std::string> notes;
};

NumericSummary summarize_numbers(std::vector<double> values);
TimelineStatistics compute_statistics(const timeline::Timeline& timeline);
std::string render_statistics(const TimelineStatistics& statistics);

} // namespace obsidiantrail::analysis

#endif
