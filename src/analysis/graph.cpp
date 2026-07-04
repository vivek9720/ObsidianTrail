#include "obsidiantrail/analysis/graph.hpp"
#include "obsidiantrail/core/text.hpp"
#include <map>
#include <sstream>

namespace obsidiantrail::analysis {

static void add_edge(CaseGraph& graph, std::string from, std::string to, std::string relation, int weight = 1) {
    if (from.empty() || to.empty()) return;
    graph.nodes.insert(from);
    graph.nodes.insert(to);
    graph.edges.push_back({std::move(from), std::move(to), std::move(relation), weight});
}

CaseGraph build_case_graph(const timeline::Timeline& timeline) {
    CaseGraph graph;
    for (const auto& event : timeline.events) {
        const auto& r = event.record;
        std::string subject = "principal:" + core::lower(r.subject);
        std::string source = "source:" + core::lower(r.source);
        std::string object = "object:" + core::lower(r.object);
        if (!r.subject.empty() && !r.source.empty()) add_edge(graph, subject, source, "observed-on", event.risk + 1);
        if (!r.subject.empty() && !r.object.empty()) add_edge(graph, subject, object, r.action.empty() ? "touched" : r.action, event.risk + 1);
        if (!r.source.empty() && !r.object.empty()) add_edge(graph, source, object, evidence::kind_name(r.kind), 1);
        for (const auto& tag : r.tags) add_edge(graph, subject, "tag:" + core::lower(tag), "tagged", 1);
    }
    return graph;
}

std::vector<std::string> neighbors(const CaseGraph& graph, const std::string& node) {
    std::vector<std::string> out;
    for (const auto& edge : graph.edges) {
        if (edge.from == node) out.push_back(edge.to);
        if (edge.to == node) out.push_back(edge.from);
    }
    return out;
}

std::vector<std::string> high_degree_nodes(const CaseGraph& graph, std::size_t threshold) {
    std::map<std::string, std::size_t> degree;
    for (const auto& edge : graph.edges) {
        degree[edge.from]++;
        degree[edge.to]++;
    }
    std::vector<std::string> out;
    for (const auto& kv : degree) {
        if (kv.second >= threshold) out.push_back(kv.first);
    }
    return out;
}

std::vector<std::string> graph_findings(const CaseGraph& graph) {
    std::vector<std::string> findings;
    for (const auto& node : high_degree_nodes(graph, 8)) findings.push_back("high-degree case node " + node);
    for (const auto& edge : graph.edges) {
        if (edge.weight >= 8 && edge.relation == "exec") findings.push_back("high-risk execution edge " + edge.from + " -> " + edge.to);
        if (core::contains_folded(edge.to, "secret") || core::contains_folded(edge.to, "password")) findings.push_back("sensitive object node " + edge.to);
    }
    return findings;
}

std::string render_graph_dot(const CaseGraph& graph) {
    std::ostringstream out;
    out << "digraph case {\n";
    for (const auto& node : graph.nodes) out << "  \"" << node << "\";\n";
    for (const auto& edge : graph.edges) out << "  \"" << edge.from << "\" -> \"" << edge.to << "\" [label=\"" << edge.relation << "\"];\n";
    out << "}\n";
    return out.str();
}

} // namespace obsidiantrail::analysis
