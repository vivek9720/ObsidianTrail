#include "obsidiantrail/core/diagnostics.hpp"
#include <sstream>

namespace obsidiantrail::core {

void Diagnostics::add(Severity severity, std::string code, std::string message, std::string source, std::size_t offset) {
    entries_.push_back({severity, std::move(code), std::move(message), std::move(source), offset});
}

void Diagnostics::info(std::string code, std::string message, std::string source, std::size_t offset) {
    add(Severity::info, std::move(code), std::move(message), std::move(source), offset);
}

void Diagnostics::warn(std::string code, std::string message, std::string source, std::size_t offset) {
    add(Severity::warning, std::move(code), std::move(message), std::move(source), offset);
}

void Diagnostics::error(std::string code, std::string message, std::string source, std::size_t offset) {
    add(Severity::error, std::move(code), std::move(message), std::move(source), offset);
}

bool Diagnostics::has_errors() const {
    for (const auto& entry : entries_) {
        if (entry.severity == Severity::error) return true;
    }
    return false;
}

std::map<std::string, std::size_t> Diagnostics::count_by_code() const {
    std::map<std::string, std::size_t> counts;
    for (const auto& entry : entries_) counts[entry.code]++;
    return counts;
}

std::map<std::string, std::size_t> Diagnostics::count_by_source() const {
    std::map<std::string, std::size_t> counts;
    for (const auto& entry : entries_) counts[entry.source]++;
    return counts;
}

std::vector<Diagnostic> Diagnostics::filter(Severity minimum) const {
    std::vector<Diagnostic> out;
    for (const auto& entry : entries_) {
        if (static_cast<int>(entry.severity) >= static_cast<int>(minimum)) out.push_back(entry);
    }
    return out;
}

std::string Diagnostics::summary() const {
    std::size_t infos = 0, warnings = 0, errors = 0;
    for (const auto& entry : entries_) {
        if (entry.severity == Severity::info) ++infos;
        else if (entry.severity == Severity::warning) ++warnings;
        else ++errors;
    }
    std::ostringstream out;
    out << "diagnostics info=" << infos << " warnings=" << warnings << " errors=" << errors;
    for (const auto& kv : count_by_code()) out << "\n" << kv.first << "=" << kv.second;
    return out.str();
}

} // namespace obsidiantrail::core
