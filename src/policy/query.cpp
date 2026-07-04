#include "obsidiantrail/policy/query.hpp"
#include "obsidiantrail/core/text.hpp"
#include <cstdlib>
#include <sstream>

namespace obsidiantrail::policy {

static Predicate parse_predicate(std::string text) {
    Predicate p;
    text = core::trim(text);
    const std::pair<std::string, Operator> ops[] = {{">=", Operator::greater_equal}, {"<=", Operator::less_equal}, {"~", Operator::contains}, {"=", Operator::equals}};
    for (const auto& op : ops) {
        auto pos = text.find(op.first);
        if (pos != std::string::npos) {
            p.field = core::normalize_key(text.substr(0, pos));
            p.op = op.second;
            p.value = core::trim(text.substr(pos + op.first.size()));
            return p;
        }
    }
    p.field = core::normalize_key(text);
    p.op = Operator::exists;
    return p;
}

Query parse_query(const std::string& text) {
    Query q;
    auto folded = core::lower(text);
    q.require_all = folded.find(" or ") == std::string::npos;
    char delim = q.require_all ? '&' : '|';
    std::string normalized = text;
    auto replace_word = [&](const std::string& word, char ch) {
        std::string needle = " " + word + " ";
        for (auto pos = core::lower(normalized).find(needle); pos != std::string::npos; pos = core::lower(normalized).find(needle)) {
            normalized.replace(pos, needle.size(), std::string(1, ch));
        }
    };
    replace_word("and", '&');
    replace_word("or", '|');
    for (auto part : core::split(normalized, delim)) {
        part = core::trim(part);
        if (!part.empty()) q.predicates.push_back(parse_predicate(part));
    }
    return q;
}

static std::string field_value(const std::string& field, const timeline::TimelineEvent& event) {
    const auto& r = event.record;
    if (field == "kind") return evidence::kind_name(r.kind);
    if (field == "source") return r.source;
    if (field == "subject") return r.subject;
    if (field == "action") return r.action;
    if (field == "object") return r.object;
    if (field == "risk") return std::to_string(event.risk);
    auto it = r.fields.find(field);
    return it == r.fields.end() ? std::string() : it->second;
}

bool matches(const Predicate& predicate, const timeline::TimelineEvent& event) {
    auto actual = field_value(predicate.field, event);
    switch (predicate.op) {
        case Operator::exists: return !actual.empty();
        case Operator::equals: return core::lower(actual) == core::lower(predicate.value);
        case Operator::contains: return core::contains_folded(actual, predicate.value);
        case Operator::greater_equal: return std::atoi(actual.c_str()) >= std::atoi(predicate.value.c_str());
        case Operator::less_equal: return std::atoi(actual.c_str()) <= std::atoi(predicate.value.c_str());
    }
    return false;
}

std::vector<timeline::TimelineEvent> evaluate(const Query& query, const timeline::Timeline& timeline) {
    std::vector<timeline::TimelineEvent> out;
    for (const auto& event : timeline.events) {
        bool any = false;
        bool all = true;
        for (const auto& predicate : query.predicates) {
            bool ok = matches(predicate, event);
            any = any || ok;
            all = all && ok;
        }
        if ((query.require_all && all) || (!query.require_all && any)) out.push_back(event);
    }
    return out;
}

std::string explain_query(const Query& query) {
    std::ostringstream out;
    out << (query.require_all ? "all" : "any") << " predicates=" << query.predicates.size();
    for (const auto& p : query.predicates) out << "\n" << p.field << " value=" << p.value;
    return out.str();
}

} // namespace obsidiantrail::policy
