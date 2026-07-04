#include <string>
#include "obsidiantrail/ledger/ledger.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){obsidiantrail::core::ByteView v(data,size); auto l=obsidiantrail::ledger::parse_ledger_page(v); (void)obsidiantrail::ledger::summarize_ledger(l); return 0;}
