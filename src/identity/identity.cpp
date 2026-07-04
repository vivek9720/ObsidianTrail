#include "obsidiantrail/identity/identity.hpp"
#include "obsidiantrail/core/text.hpp"
#include <set>
#include <sstream>

namespace obsidiantrail::identity {

Principal parse_principal(const std::string& text) {
    Principal p;
    auto raw = core::trim(text);
    auto at = raw.find('@');
    auto slash = raw.find('/');
    if (at != std::string::npos) {
        p.name = raw.substr(0, at);
        p.realm = raw.substr(at + 1);
    } else if (slash != std::string::npos) {
        p.realm = raw.substr(0, slash);
        p.name = raw.substr(slash + 1);
    } else {
        p.name = raw;
    }
    p.name = core::lower(core::trim(p.name));
    p.realm = core::upper(core::trim(p.realm));
    if (p.name == "root" || p.name == "administrator" || p.name == "admin") p.roles.push_back("administrator");
    if (core::ends_with(p.name, "$")) p.roles.push_back("machine");
    return p;
}

Session session_from_record(const evidence::EvidenceRecord& record) {
    Session s;
    auto get = [&](const std::string& key) -> std::string {
        auto it = record.fields.find(core::normalize_key(key));
        return it == record.fields.end() ? std::string() : it->second;
    };
    s.id = get("session");
    if (s.id.empty()) s.id = get("session_id");
    if (s.id.empty()) s.id = core::hex64(core::stable_hash64(record.raw));
    s.principal = parse_principal(record.subject.empty() ? get("user") : record.subject);
    for (const auto& role : core::split(get("roles"), ',')) {
        auto clean = core::lower(core::trim(role));
        if (!clean.empty()) s.principal.roles.push_back(clean);
    }
    for (const auto& group : core::split(get("groups"), ',')) {
        auto clean = core::lower(core::trim(group));
        if (!clean.empty()) s.principal.groups.push_back(clean);
    }
    s.host = get("host");
    if (s.host.empty()) s.host = record.source;
    s.source_ip = get("src");
    if (s.source_ip.empty()) s.source_ip = get("source_ip");
    s.start = record.timestamp;
    s.end = record.timestamp;
    for (const auto& role : s.principal.roles) {
        if (role == "administrator" || role == "root" || role == "domain_admin") s.privileged = true;
    }
    if (core::contains_folded(record.raw, "stale") || core::contains_folded(record.raw, "expired")) s.stale = true;
    return s;
}

std::vector<Session> collect_sessions(const std::vector<evidence::EvidenceRecord>& records) {
    std::map<std::string, Session> sessions;
    for (const auto& record : records) {
        if (record.kind != evidence::RecordKind::identity && record.kind != evidence::RecordKind::process) continue;
        auto s = session_from_record(record);
        auto it = sessions.find(s.id);
        if (it == sessions.end()) {
            sessions[s.id] = s;
        } else {
            if (s.start < it->second.start) it->second.start = s.start;
            if (it->second.end < s.end) it->second.end = s.end;
            it->second.privileged = it->second.privileged || s.privileged;
            it->second.stale = it->second.stale || s.stale;
        }
    }
    std::vector<Session> out;
    for (auto& kv : sessions) out.push_back(std::move(kv.second));
    return out;
}

std::vector<std::string> identity_findings(const std::vector<Session>& sessions) {
    std::vector<std::string> findings;
    std::map<std::string, std::set<std::string>> user_ips;
    for (const auto& s : sessions) {
        user_ips[s.principal.name].insert(s.source_ip);
        if (s.privileged && s.stale) findings.push_back("stale privileged session " + s.id);
        if (s.privileged && s.host.empty()) findings.push_back("privileged session without host " + s.id);
        if (s.principal.name.empty()) findings.push_back("anonymous session " + s.id);
    }
    for (const auto& kv : user_ips) {
        if (kv.second.size() > 4) findings.push_back("principal uses many source addresses " + kv.first);
    }
    return findings;
}

std::string summarize_principal(const Principal& principal) {
    std::ostringstream out;
    out << principal.name;
    if (!principal.realm.empty()) out << "@" << principal.realm;
    if (!principal.roles.empty()) out << " roles=" << core::join(principal.roles, ",");
    if (!principal.groups.empty()) out << " groups=" << core::join(principal.groups, ",");
    return out.str();
}

} // namespace obsidiantrail::identity
