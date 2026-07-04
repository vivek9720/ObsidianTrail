#include <string>
#include "obsidiantrail/evidence/bundle.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto b=obsidiantrail::evidence::parse_bundle_text(s); (void)obsidiantrail::evidence::summarize_bundle(b); return 0;}
