#include <string>
#include "obsidiantrail/redaction/redactor.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); auto r=obsidiantrail::redaction::redact(s); (void)obsidiantrail::redaction::summarize_redactions(r); return 0;}
