#include "obsidiantrail/redaction/redactor.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace obsidiantrail::redaction {

static bool looks_token_char(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_' || ch == '-' || ch == '.' || ch == '/';
}

std::vector<Redaction> scan_sensitive(const std::string& text) {
    std::vector<Redaction> out;
    const std::string keys[] = {"password", "passwd", "secret", "token", "apikey", "api_key", "authorization"};
    std::string folded = text;
    std::transform(folded.begin(), folded.end(), folded.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    for (const auto& key : keys) {
        std::size_t pos = 0;
        while ((pos = folded.find(key, pos)) != std::string::npos) {
            auto value = text.find_first_of("=:", pos + key.size());
            if (value != std::string::npos) {
                ++value;
                while (value < text.size() && std::isspace(static_cast<unsigned char>(text[value]))) ++value;
                std::size_t end = value;
                while (end < text.size() && looks_token_char(text[end])) ++end;
                if (end > value) out.push_back({value, end - value, key});
            }
            pos += key.size();
        }
    }
    for (std::size_t i = 0; i + 15 < text.size(); ++i) {
        if (text.compare(i, 3, "AKI") == 0 || text.compare(i, 3, "sk_") == 0) {
            std::size_t end = i;
            while (end < text.size() && looks_token_char(text[end])) ++end;
            if (end - i >= 16) out.push_back({i, end - i, "credential"});
            i = end;
        }
    }
    std::sort(out.begin(), out.end(), [](const Redaction& a, const Redaction& b) { return a.offset < b.offset; });
    return out;
}

RedactionResult redact(const std::string& text) {
    RedactionResult result;
    result.text = text;
    result.redactions = scan_sensitive(text);
    for (const auto& r : result.redactions) {
        if (r.offset >= result.text.size()) continue;
        std::size_t end = std::min(result.text.size(), r.offset + r.length);
        for (std::size_t i = r.offset; i < end; ++i) result.text[i] = '*';
    }
    return result;
}

std::string summarize_redactions(const RedactionResult& result) {
    std::ostringstream out;
    out << "redactions=" << result.redactions.size();
    std::size_t bytes = 0;
    for (const auto& r : result.redactions) bytes += r.length;
    out << " bytes=" << bytes;
    return out.str();
}

} // namespace obsidiantrail::redaction
