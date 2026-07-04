#include <string>
#include "obsidiantrail/evidence/manifest.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto m=obsidiantrail::evidence::parse_manifest_text(s); (void)obsidiantrail::evidence::summarize_manifest(m); return 0;}
