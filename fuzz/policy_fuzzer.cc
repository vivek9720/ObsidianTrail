#include <string>
#include "obsidiantrail/evidence/record.hpp"
#include "obsidiantrail/policy/query.hpp"
#include "obsidiantrail/timeline/timeline.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto q=obsidiantrail::policy::parse_query(s); auto records=obsidiantrail::evidence::parse_record_text("kind=alert; subject=alice; action=signal; time=1\n","policy"); auto t=obsidiantrail::timeline::build_timeline(records); (void)obsidiantrail::policy::evaluate(q,t); (void)obsidiantrail::policy::explain_query(q); return 0;}
