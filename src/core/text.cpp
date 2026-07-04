#include "obsidiantrail/core/text.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace obsidiantrail::core {

std::string trim(std::string value) {
    auto space = [](unsigned char c) { return std::isspace(c) != 0; };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [&](char c) { return !space(static_cast<unsigned char>(c)); }));
    value.erase(std::find_if(value.rbegin(), value.rend(), [&](char c) { return !space(static_cast<unsigned char>(c)); }).base(), value.end());
    return value;
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string upper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

std::vector<std::string> split(const std::string& text, char delim) {
    std::vector<std::string> out;
    std::string part;
    for (char ch : text) {
        if (ch == delim) {
            out.push_back(part);
            part.clear();
        } else {
            part.push_back(ch);
        }
    }
    out.push_back(part);
    return out;
}

std::vector<std::string> lines(const std::string& text) {
    std::vector<std::string> out;
    std::string part;
    for (char ch : text) {
        if (ch == '\n') {
            if (!part.empty() && part.back() == '\r') part.pop_back();
            out.push_back(part);
            part.clear();
        } else {
            part.push_back(ch);
        }
    }
    if (!part.empty() || (!text.empty() && text.back() == '\n')) out.push_back(part);
    return out;
}

std::string join(const std::vector<std::string>& parts, const std::string& delim) {
    std::ostringstream out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i) out << delim;
        out << parts[i];
    }
    return out.str();
}

bool starts_with(const std::string& text, const std::string& prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

bool ends_with(const std::string& text, const std::string& suffix) {
    return text.size() >= suffix.size() && text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool contains_folded(const std::string& text, const std::string& needle) {
    return lower(text).find(lower(needle)) != std::string::npos;
}

std::string normalize_key(std::string key) {
    key = lower(trim(key));
    for (char& ch : key) {
        if (ch == '-' || ch == ' ' || ch == '.') ch = '_';
    }
    return key;
}

std::map<std::string, std::string> parse_pairs(const std::string& text) {
    std::map<std::string, std::string> out;
    for (const auto& raw : split(text, ';')) {
        auto part = trim(raw);
        if (part.empty()) continue;
        auto eq = part.find('=');
        if (eq == std::string::npos) eq = part.find(':');
        if (eq == std::string::npos) {
            out[normalize_key(part)] = "true";
        } else {
            out[normalize_key(part.substr(0, eq))] = trim(part.substr(eq + 1));
        }
    }
    return out;
}

static int hex_value(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return 10 + ch - 'a';
    if (ch >= 'A' && ch <= 'F') return 10 + ch - 'A';
    return -1;
}

std::string percent_decode_lossy(const std::string& text) {
    std::string out;
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '%' && i + 2 < text.size()) {
            int a = hex_value(text[i + 1]);
            int b = hex_value(text[i + 2]);
            if (a >= 0 && b >= 0) {
                out.push_back(static_cast<char>((a << 4) | b));
                i += 2;
                continue;
            }
        }
        out.push_back(text[i] == '+' ? ' ' : text[i]);
    }
    return out;
}

std::string collapse_space(const std::string& text) {
    std::string out;
    bool last_space = false;
    for (unsigned char ch : text) {
        bool now = std::isspace(ch) != 0;
        if (now && !last_space) out.push_back(' ');
        if (!now) out.push_back(static_cast<char>(ch));
        last_space = now;
    }
    return trim(out);
}

std::uint32_t stable_hash32(const std::string& text) {
    std::uint32_t h = 2166136261u;
    for (unsigned char ch : text) {
        h ^= ch;
        h *= 16777619u;
    }
    return h;
}

std::uint64_t stable_hash64(const std::string& text) {
    std::uint64_t h = 1469598103934665603ull;
    for (unsigned char ch : text) {
        h ^= ch;
        h *= 1099511628211ull;
    }
    return h;
}

std::string hex64(std::uint64_t value) {
    std::ostringstream out;
    out << std::hex << std::setw(16) << std::setfill('0') << value;
    return out.str();
}

} // namespace obsidiantrail::core
