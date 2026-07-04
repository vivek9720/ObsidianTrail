#include "obsidiantrail/timeline/timeline.hpp"
#include "obsidiantrail/core/text.hpp"
#include <algorithm>
#include <sstream>

namespace obsidiantrail::timeline {

static int base_risk(const evidence::EvidenceRecord& r, std::vector<std::string>& reasons) {
    int risk = 0;
    auto raw = core::lower(r.raw);
    if (r.kind == evidence::RecordKind::alert) { risk += 4; reasons.push_back("alert record"); }
    if (core::contains_folded(raw, "admin") || core::contains_folded(raw, "root")) { risk += 3; reasons.push_back("privileged term"); }
    if (core::contains_folded(raw, "failed") || core::contains_folded(raw, "denied")) { risk += 2; reasons.push_back("failure term"); }
    if (core::contains_folded(raw, "powershell") || core::contains_folded(raw, "rundll32")) { risk += 3; reasons.push_back("living-off-land process"); }
    if (core::contains_folded(raw, "public") || core::contains_folded(raw, "external")) { risk += 1; reasons.push_back("external boundary"); }
    return risk;
}

Timeline build_timeline(const std::vector<evidence::EvidenceRecord>& records) {
    Timeline timeline;
    for (const auto& record : records) {
        TimelineEvent event;
        event.record = record;
        event.risk = base_risk(record, event.reasons);
        timeline.risk_by_subject[core::lower(record.subject)] += event.risk;
        timeline.events.push_back(std::move(event));
    }
    std::stable_sort(timeline.events.begin(), timeline.events.end(), [](const TimelineEvent& a, const TimelineEvent& b) {
        return a.record.timestamp < b.record.timestamp;
    });
    timeline.findings = correlate(timeline);
    return timeline;
}

std::vector<TimelineEvent> window(const Timeline& timeline, core::Timestamp start, core::Timestamp end) {
    std::vector<TimelineEvent> out;
    for (const auto& event : timeline.events) {
        if (!(event.record.timestamp < start) && !(end < event.record.timestamp)) out.push_back(event);
    }
    return out;
}

std::vector<std::string> correlate(const Timeline& timeline) {
    std::vector<std::string> findings;
    for (std::size_t i = 1; i < timeline.events.size(); ++i) {
        const auto& prev = timeline.events[i - 1].record;
        const auto& cur = timeline.events[i].record;
        auto gap = core::seconds_between(prev.timestamp, cur.timestamp);
        if (prev.subject == cur.subject && prev.kind == evidence::RecordKind::identity && cur.kind == evidence::RecordKind::process && gap >= 0 && gap < 60) {
            findings.push_back("process shortly after identity event for " + cur.subject);
        }
        if (prev.source == cur.source && prev.kind == evidence::RecordKind::network && cur.kind == evidence::RecordKind::file && gap >= 0 && gap < 30) {
            findings.push_back("file activity shortly after network activity on " + cur.source);
        }
    }
    for (const auto& kv : timeline.risk_by_subject) {
        if (!kv.first.empty() && kv.second >= 10) findings.push_back("high cumulative subject risk " + kv.first);
    }
    return findings;
}

std::string summarize_timeline(const Timeline& timeline) {
    std::ostringstream out;
    out << "events=" << timeline.events.size() << " subjects=" << timeline.risk_by_subject.size();
    for (const auto& finding : timeline.findings) out << "\nfinding: " << finding;
    return out.str();
}

} // namespace obsidiantrail::timeline
