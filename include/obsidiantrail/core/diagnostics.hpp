#ifndef OBSIDIANTRAIL_CORE_DIAGNOSTICS_HPP
#define OBSIDIANTRAIL_CORE_DIAGNOSTICS_HPP

#include "obsidiantrail/core/result.hpp"
#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::core {

struct Diagnostic {
    Severity severity = Severity::info;
    std::string code;
    std::string message;
    std::string source;
    std::size_t offset = 0;
};

class Diagnostics {
public:
    void add(Severity severity, std::string code, std::string message, std::string source = {}, std::size_t offset = 0);
    void info(std::string code, std::string message, std::string source = {}, std::size_t offset = 0);
    void warn(std::string code, std::string message, std::string source = {}, std::size_t offset = 0);
    void error(std::string code, std::string message, std::string source = {}, std::size_t offset = 0);
    bool has_errors() const;
    bool empty() const { return entries_.empty(); }
    std::size_t size() const { return entries_.size(); }
    const std::vector<Diagnostic>& entries() const { return entries_; }
    std::map<std::string, std::size_t> count_by_code() const;
    std::map<std::string, std::size_t> count_by_source() const;
    std::vector<Diagnostic> filter(Severity minimum) const;
    std::string summary() const;
private:
    std::vector<Diagnostic> entries_;
};

} // namespace obsidiantrail::core

#endif
