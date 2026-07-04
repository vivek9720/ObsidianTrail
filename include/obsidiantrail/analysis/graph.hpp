#ifndef OBSIDIANTRAIL_ANALYSIS_GRAPH_HPP
#define OBSIDIANTRAIL_ANALYSIS_GRAPH_HPP

#include "obsidiantrail/timeline/timeline.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace obsidiantrail::analysis {

struct Edge {
    std::string from;
    std::string to;
    std::string relation;
    int weight = 1;
};

struct CaseGraph {
    std::set<std::string> nodes;
    std::vector<Edge> edges;
};

CaseGraph build_case_graph(const timeline::Timeline& timeline);
std::vector<std::string> neighbors(const CaseGraph& graph, const std::string& node);
std::vector<std::string> high_degree_nodes(const CaseGraph& graph, std::size_t threshold);
std::vector<std::string> graph_findings(const CaseGraph& graph);
std::string render_graph_dot(const CaseGraph& graph);

} // namespace obsidiantrail::analysis

#endif
