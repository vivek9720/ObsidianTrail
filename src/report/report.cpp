#include "obsidiantrail/report/report.hpp"
#include "obsidiantrail/core/text.hpp"
#include <sstream>

namespace obsidiantrail::report {

CaseReport build_report(const evidence::Manifest& manifest, const timeline::Timeline& timeline) {
    CaseReport report;
    report.title = "Case " + (manifest.case_id.empty() ? std::string("unknown") : manifest.case_id);
    int risk = 0;
    for (const auto& event : timeline.events) risk += event.risk;
    risk += static_cast<int>(timeline.findings.size() * 3);
    risk += static_cast<int>(manifest.warnings.size());
    report.risk = risk;
    std::ostringstream summary;
    summary << "entries=" << manifest.entries.size() << " events=" << timeline.events.size() << " findings=" << timeline.findings.size();
    report.summary = summary.str();
    std::ostringstream details;
    for (const auto& finding : timeline.findings) details << "finding: " << finding << "\n";
    for (const auto& warning : manifest.warnings) details << "manifest: " << warning << "\n";
    report.details = details.str();
    return report;
}

std::string render_text(const CaseReport& report) {
    std::ostringstream out;
    out << report.title << "\n";
    out << "risk=" << report.risk << "\n";
    out << report.summary << "\n";
    out << report.details;
    return out.str();
}

static std::string json_escape(const std::string& text) {
    std::string out;
    for (char ch : text) {
        if (ch == '\\' || ch == '"') { out.push_back('\\'); out.push_back(ch); }
        else if (ch == '\n') out += "\\n";
        else out.push_back(ch);
    }
    return out;
}

std::string render_json(const CaseReport& report) {
    std::ostringstream out;
    out << "{";
    out << "\"title\":\"" << json_escape(report.title) << "\",";
    out << "\"risk\":" << report.risk << ",";
    out << "\"summary\":\"" << json_escape(report.summary) << "\",";
    out << "\"details\":\"" << json_escape(report.details) << "\"";
    out << "}";
    return out.str();
}

} // namespace obsidiantrail::report
