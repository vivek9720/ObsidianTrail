#include "obsidiantrail/evidence/bundle.hpp"
#include "obsidiantrail/policy/query.hpp"
#include "obsidiantrail/timeline/timeline.hpp"
#include <cassert>

int main() {
    const char* text = "[manifest]\ncase=demo; created_by=test\nentry=events.otl; type=log; digest=abc; size=12\n[records]\nkind=identity; time=100; source=vpn; subject=alice; action=login; src=198.51.100.2\nkind=process; time=120; source=laptop; subject=alice; action=exec; object=powershell.exe\n";
    auto bundle = obsidiantrail::evidence::parse_bundle_text(text);
    assert(bundle.records.size() == 2);
    auto timeline = obsidiantrail::timeline::build_timeline(bundle.records);
    auto query = obsidiantrail::policy::parse_query("subject=alice and risk>=1");
    auto matches = obsidiantrail::policy::evaluate(query, timeline);
    assert(!matches.empty());
    return 0;
}
