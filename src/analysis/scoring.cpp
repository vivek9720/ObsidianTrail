#include "obsidiantrail/analysis/scoring.hpp"
#include "obsidiantrail/core/text.hpp"
#include <sstream>

namespace obsidiantrail::analysis {

static void add(ScoreCard& card, std::string code, std::string message, int points) {
    card.total += points;
    card.signals.push_back({std::move(code), std::move(message), points});
}

ScoreCard score_timeline(const timeline::Timeline& timeline) {
    ScoreCard card;
    for (const auto& event : timeline.events) {
        if (event.risk >= 6) add(card, "timeline.high_event", "high-risk event " + evidence::render_record(event.record), event.risk);
        if (event.record.kind == evidence::RecordKind::unknown) add(card, "timeline.unknown_kind", "unknown event kind", 1);
        if (event.record.source.empty()) add(card, "timeline.missing_source", "event missing source", 1);
    }
    for (const auto& finding : timeline.findings) add(card, "timeline.correlation", finding, 3);
    return card;
}

ScoreCard score_sessions(const std::vector<identity::Session>& sessions) {
    ScoreCard card;
    for (const auto& session : sessions) {
        if (session.privileged) add(card, "identity.privileged", "privileged session " + session.id, 3);
        if (session.stale) add(card, "identity.stale", "stale session " + session.id, 2);
        if (session.privileged && session.source_ip.empty()) add(card, "identity.no_source", "privileged session without source IP", 2);
        if (session.principal.name == "administrator" || session.principal.name == "root") add(card, "identity.builtin_admin", "built-in administrator account", 2);
    }
    return card;
}

ScoreCard score_flows(const std::vector<network::Flow>& flows) {
    ScoreCard card;
    for (const auto& flow : flows) {
        auto src = network::classify_ip(flow.src);
        auto dst = network::classify_ip(flow.dst);
        if (src == "public" && (flow.dst_port == 22 || flow.dst_port == 3389 || flow.dst_port == 5985)) add(card, "network.admin_inbound", "public administrative inbound flow", 5);
        if (dst == "public" && flow.dst_port == 3389) add(card, "network.rdp_public", "RDP to public destination", 4);
        if (flow.protocol.empty()) add(card, "network.no_protocol", "flow without protocol", 1);
        if (flow.dst_port == 0) add(card, "network.zero_port", "flow with zero destination port", 1);
    }
    return card;
}

ScoreCard score_graph(const CaseGraph& graph) {
    ScoreCard card;
    for (const auto& finding : graph_findings(graph)) add(card, "graph.finding", finding, 3);
    for (const auto& edge : graph.edges) {
        if (edge.weight > 10) add(card, "graph.heavy_edge", edge.from + " -> " + edge.to, 2);
    }
    return card;
}

ScoreCard merge_scores(std::vector<ScoreCard> cards) {
    ScoreCard merged;
    for (auto& card : cards) {
        merged.total += card.total;
        for (auto& signal : card.signals) merged.signals.push_back(std::move(signal));
    }
    return merged;
}

std::string summarize_score(const ScoreCard& score) {
    std::ostringstream out;
    out << "score=" << score.total << " signals=" << score.signals.size();
    for (const auto& signal : score.signals) out << "\n" << signal.code << ":" << signal.points << ":" << signal.message;
    return out.str();
}

} // namespace obsidiantrail::analysis
