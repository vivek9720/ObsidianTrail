#include "obsidiantrail/evidence/bundle.hpp"
#include "obsidiantrail/report/report.hpp"
#include "obsidiantrail/timeline/timeline.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: otinspect <bundle-file>\n";
        return 2;
    }
    std::ifstream in(argv[1], std::ios::binary);
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    auto bundle = obsidiantrail::evidence::parse_bundle_text(text);
    auto timeline = obsidiantrail::timeline::build_timeline(bundle.records);
    auto report = obsidiantrail::report::build_report(bundle.manifest, timeline);
    std::cout << obsidiantrail::report::render_text(report);
    return 0;
}
