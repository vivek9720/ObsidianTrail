#ifndef OBSIDIANTRAIL_REDACTION_REDACTOR_HPP
#define OBSIDIANTRAIL_REDACTION_REDACTOR_HPP

#include <string>
#include <vector>

namespace obsidiantrail::redaction {

struct Redaction {
    std::size_t offset = 0;
    std::size_t length = 0;
    std::string label;
};

struct RedactionResult {
    std::string text;
    std::vector<Redaction> redactions;
};

std::vector<Redaction> scan_sensitive(const std::string& text);
RedactionResult redact(const std::string& text);
std::string summarize_redactions(const RedactionResult& result);

} // namespace obsidiantrail::redaction

#endif
