#ifndef OBSIDIANTRAIL_POLICY_QUERY_HPP
#define OBSIDIANTRAIL_POLICY_QUERY_HPP

#include "obsidiantrail/timeline/timeline.hpp"
#include <string>
#include <vector>

namespace obsidiantrail::policy {

enum class Operator { exists, equals, contains, greater_equal, less_equal };

struct Predicate {
    std::string field;
    Operator op = Operator::exists;
    std::string value;
};

struct Query {
    std::vector<Predicate> predicates;
    bool require_all = true;
};

Query parse_query(const std::string& text);
bool matches(const Predicate& predicate, const timeline::TimelineEvent& event);
std::vector<timeline::TimelineEvent> evaluate(const Query& query, const timeline::Timeline& timeline);
std::string explain_query(const Query& query);

} // namespace obsidiantrail::policy

#endif
