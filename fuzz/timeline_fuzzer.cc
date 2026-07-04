#include <string>
#include "obsidiantrail/timeline/timeline.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto r=obsidiantrail::evidence::parse_record_text(s,"timeline"); auto t=obsidiantrail::timeline::build_timeline(r); (void)obsidiantrail::timeline::summarize_timeline(t); return 0;}
