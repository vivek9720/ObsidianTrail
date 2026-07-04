#ifndef OBSIDIANTRAIL_ANALYSIS_SCORING_HPP
#define OBSIDIANTRAIL_ANALYSIS_SCORING_HPP

#include "obsidiantrail/analysis/graph.hpp"
#include "obsidiantrail/identity/identity.hpp"
#include "obsidiantrail/network/network.hpp"
#include <string>
#include <vector>

namespace obsidiantrail::analysis {

struct ScoreSignal {
    std::string code;
    std::string message;
    int points = 0;
};

struct ScoreCard {
    int total = 0;
    std::vector<ScoreSignal> signals;
};

ScoreCard score_timeline(const timeline::Timeline& timeline);
ScoreCard score_sessions(const std::vector<identity::Session>& sessions);
ScoreCard score_flows(const std::vector<network::Flow>& flows);
ScoreCard score_graph(const CaseGraph& graph);
ScoreCard merge_scores(std::vector<ScoreCard> cards);
std::string summarize_score(const ScoreCard& score);

} // namespace obsidiantrail::analysis

#endif
