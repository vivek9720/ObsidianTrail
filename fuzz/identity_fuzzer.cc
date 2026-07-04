#include <string>
#include "obsidiantrail/evidence/record.hpp"
#include "obsidiantrail/identity/identity.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto p=obsidiantrail::identity::parse_principal(s); (void)obsidiantrail::identity::summarize_principal(p); auto r=obsidiantrail::evidence::parse_record_text(s,"id"); auto ss=obsidiantrail::identity::collect_sessions(r); (void)obsidiantrail::identity::identity_findings(ss); return 0;}
