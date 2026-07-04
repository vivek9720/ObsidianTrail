#include "obsidiantrail/analysis/statistics.hpp"
#include "obsidiantrail/core/text.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>

namespace obsidiantrail::analysis {

NumericSummary summarize_numbers(std::vector<double> values) {
    NumericSummary summary;
    if (values.empty()) return summary;
    std::sort(values.begin(), values.end());
    summary.min = values.front();
    summary.max = values.back();
    summary.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    summary.median = values[values.size() / 2];
    summary.p95 = values[std::min(values.size() - 1, static_cast<std::size_t>(values.size() * 0.95))];
    return summary;
}

TimelineStatistics compute_statistics(const timeline::Timeline& timeline) {
    TimelineStatistics stats;
    std::vector<double> risks;
    for (const auto& event : timeline.events) {
        stats.events_by_kind[evidence::kind_name(event.record.kind)]++;
        stats.events_by_source[event.record.source]++;
        if (!event.record.subject.empty() && !event.record.action.empty()) {
            stats.actions_by_subject[core::lower(event.record.subject) + ":" + core::lower(event.record.action)]++;
        }
        risks.push_back(static_cast<double>(event.risk));
    }
    stats.risk = summarize_numbers(risks);
    for (const auto& kv : stats.events_by_source) {
        if (kv.second > 1000) stats.notes.push_back("high volume source " + kv.first);
    }
    for (const auto& kv : stats.events_by_kind) {
        if (kv.first == "unknown" && kv.second > 0) stats.notes.push_back("unknown event kinds present");
    }
    return stats;
}

std::string render_statistics(const TimelineStatistics& statistics) {
    std::ostringstream out;
    out << "risk min=" << statistics.risk.min << " mean=" << statistics.risk.mean << " p95=" << statistics.risk.p95 << " max=" << statistics.risk.max;
    out << "\nby_kind";
    for (const auto& kv : statistics.events_by_kind) out << " " << kv.first << "=" << kv.second;
    out << "\nby_source";
    for (const auto& kv : statistics.events_by_source) out << " " << kv.first << "=" << kv.second;
    for (const auto& note : statistics.notes) out << "\nnote: " << note;
    return out.str();
}

} // namespace obsidiantrail::analysis
