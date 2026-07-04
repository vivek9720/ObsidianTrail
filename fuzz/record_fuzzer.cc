#include <string>
#include "obsidiantrail/evidence/record.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto r=obsidiantrail::evidence::parse_record_text(s,"fuzz"); for(auto& e:r){(void)obsidiantrail::evidence::validate_record(e); (void)obsidiantrail::evidence::render_record(e);} return 0;}
