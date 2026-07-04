#include <string>
#include "obsidiantrail/report/report.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto b=obsidiantrail::evidence::parse_bundle_text(s); auto t=obsidiantrail::timeline::build_timeline(b.records); auto r=obsidiantrail::report::build_report(b.manifest,t); (void)obsidiantrail::report::render_json(r); (void)obsidiantrail::report::render_text(r); return 0;}
