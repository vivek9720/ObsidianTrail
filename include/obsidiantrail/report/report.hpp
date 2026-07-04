#ifndef OBSIDIANTRAIL_REPORT_REPORT_HPP
#define OBSIDIANTRAIL_REPORT_REPORT_HPP

#include "obsidiantrail/evidence/manifest.hpp"
#include "obsidiantrail/timeline/timeline.hpp"
#include <string>

namespace obsidiantrail::report {

struct CaseReport {
    std::string title;
    std::string summary;
    std::string details;
    int risk = 0;
};

CaseReport build_report(const evidence::Manifest& manifest, const timeline::Timeline& timeline);
std::string render_text(const CaseReport& report);
std::string render_json(const CaseReport& report);

} // namespace obsidiantrail::report

#endif
