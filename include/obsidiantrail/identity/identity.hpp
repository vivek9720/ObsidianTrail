#ifndef OBSIDIANTRAIL_IDENTITY_IDENTITY_HPP
#define OBSIDIANTRAIL_IDENTITY_IDENTITY_HPP

#include "obsidiantrail/evidence/record.hpp"
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::identity {

struct Principal {
    std::string name;
    std::string realm;
    std::vector<std::string> roles;
    std::vector<std::string> groups;
    std::map<std::string, std::string> claims;
};

struct Session {
    std::string id;
    Principal principal;
    std::string host;
    std::string source_ip;
    core::Timestamp start;
    core::Timestamp end;
    bool privileged = false;
    bool stale = false;
};

Principal parse_principal(const std::string& text);
Session session_from_record(const evidence::EvidenceRecord& record);
std::vector<Session> collect_sessions(const std::vector<evidence::EvidenceRecord>& records);
std::vector<std::string> identity_findings(const std::vector<Session>& sessions);
std::string summarize_principal(const Principal& principal);

} // namespace obsidiantrail::identity

#endif
