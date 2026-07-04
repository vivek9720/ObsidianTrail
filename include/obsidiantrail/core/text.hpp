#ifndef OBSIDIANTRAIL_CORE_TEXT_HPP
#define OBSIDIANTRAIL_CORE_TEXT_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::core {

std::string trim(std::string value);
std::string lower(std::string value);
std::string upper(std::string value);
std::vector<std::string> split(const std::string& text, char delim);
std::vector<std::string> lines(const std::string& text);
std::string join(const std::vector<std::string>& parts, const std::string& delim);
bool starts_with(const std::string& text, const std::string& prefix);
bool ends_with(const std::string& text, const std::string& suffix);
bool contains_folded(const std::string& text, const std::string& needle);
std::string normalize_key(std::string key);
std::map<std::string, std::string> parse_pairs(const std::string& text);
std::string percent_decode_lossy(const std::string& text);
std::string collapse_space(const std::string& text);
std::uint32_t stable_hash32(const std::string& text);
std::uint64_t stable_hash64(const std::string& text);
std::string hex64(std::uint64_t value);

} // namespace obsidiantrail::core

#endif
