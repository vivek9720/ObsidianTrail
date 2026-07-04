#include "obsidiantrail/core/time.hpp"
#include "obsidiantrail/core/text.hpp"
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>

namespace obsidiantrail::core {

Timestamp normalize_time(std::int64_t seconds, std::int64_t nanos, std::string source) {
    while (nanos >= 1000000000LL) { ++seconds; nanos -= 1000000000LL; }
    while (nanos < 0) { --seconds; nanos += 1000000000LL; }
    return Timestamp{seconds, static_cast<std::int32_t>(nanos), std::move(source)};
}

bool Timestamp::operator<(const Timestamp& other) const {
    return seconds < other.seconds || (seconds == other.seconds && nanos < other.nanos);
}

std::string Timestamp::to_string() const {
    std::ostringstream out;
    out << seconds;
    if (nanos) out << "." << std::setw(9) << std::setfill('0') << nanos;
    return out.str();
}

Result<Timestamp> parse_unix_time(const std::string& text) {
    auto s = trim(text);
    if (s.empty()) return Result<Timestamp>::err("time.empty", "empty timestamp");
    char* end = nullptr;
    long long seconds = std::strtoll(s.c_str(), &end, 10);
    if (end == s.c_str()) return Result<Timestamp>::err("time.number", "timestamp is not numeric");
    long long nanos = 0;
    if (*end == '.') {
        ++end;
        int digits = 0;
        while (*end >= '0' && *end <= '9' && digits < 9) {
            nanos = nanos * 10 + (*end - '0');
            ++end;
            ++digits;
        }
        while (digits++ < 9) nanos *= 10;
    }
    return Result<Timestamp>::ok(normalize_time(seconds, nanos, "unix"));
}

Result<Timestamp> parse_iso8601(const std::string& text) {
    int y = 0, m = 0, d = 0, hh = 0, mm = 0, ss = 0;
    if (std::sscanf(text.c_str(), "%d-%d-%dT%d:%d:%d", &y, &m, &d, &hh, &mm, &ss) != 6) {
        return Result<Timestamp>::err("time.iso", "not an ISO-8601 UTC timestamp");
    }
    if (m < 1 || m > 12 || d < 1 || d > 31 || hh < 0 || hh > 23 || mm < 0 || mm > 59 || ss < 0 || ss > 60) {
        return Result<Timestamp>::err("time.range", "timestamp field out of range");
    }
    static const int days_before_month[] = {0,0,31,59,90,120,151,181,212,243,273,304,334};
    auto leap = [](int year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); };
    long long days = 0;
    for (int year = 1970; year < y; ++year) days += leap(year) ? 366 : 365;
    days += days_before_month[m] + d - 1;
    if (m > 2 && leap(y)) ++days;
    return Result<Timestamp>::ok(normalize_time(days * 86400LL + hh * 3600LL + mm * 60LL + ss, 0, "iso8601"));
}

std::int64_t seconds_between(const Timestamp& left, const Timestamp& right) {
    return right.seconds - left.seconds;
}

std::string duration_label(std::int64_t seconds) {
    std::ostringstream out;
    if (seconds < 0) { out << "-"; seconds = -seconds; }
    if (seconds >= 86400) out << (seconds / 86400) << "d";
    else if (seconds >= 3600) out << (seconds / 3600) << "h";
    else if (seconds >= 60) out << (seconds / 60) << "m";
    else out << seconds << "s";
    return out.str();
}

} // namespace obsidiantrail::core
