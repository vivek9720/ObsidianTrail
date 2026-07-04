#ifndef OBSIDIANTRAIL_CORE_TIME_HPP
#define OBSIDIANTRAIL_CORE_TIME_HPP

#include "obsidiantrail/core/result.hpp"
#include <cstdint>
#include <string>

namespace obsidiantrail::core {

struct Timestamp {
    std::int64_t seconds = 0;
    std::int32_t nanos = 0;
    std::string source;
    std::string to_string() const;
    bool operator<(const Timestamp& other) const;
};

Result<Timestamp> parse_unix_time(const std::string& text);
Result<Timestamp> parse_iso8601(const std::string& text);
Timestamp normalize_time(std::int64_t seconds, std::int64_t nanos, std::string source = {});
std::int64_t seconds_between(const Timestamp& left, const Timestamp& right);
std::string duration_label(std::int64_t seconds);

} // namespace obsidiantrail::core

#endif
