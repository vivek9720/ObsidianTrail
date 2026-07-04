#include "obsidiantrail/network/network.hpp"
#include "obsidiantrail/core/text.hpp"
#include <cstdlib>
#include <map>
#include <sstream>

namespace obsidiantrail::network {

std::optional<IPv4> parse_ipv4(const std::string& text) {
    auto parts = core::split(core::trim(text), '.');
    if (parts.size() != 4) return std::nullopt;
    std::uint32_t value = 0;
    for (const auto& part : parts) {
        if (part.empty() || part.size() > 3) return std::nullopt;
        char* end = nullptr;
        long octet = std::strtol(part.c_str(), &end, 10);
        if (*end != '\0' || octet < 0 || octet > 255) return std::nullopt;
        value = (value << 8) | static_cast<std::uint32_t>(octet);
    }
    return IPv4{value};
}

std::string ipv4_to_string(IPv4 ip) {
    std::ostringstream out;
    out << ((ip.value >> 24) & 255) << "." << ((ip.value >> 16) & 255) << "." << ((ip.value >> 8) & 255) << "." << (ip.value & 255);
    return out.str();
}

bool Cidr::contains(IPv4 ip) const {
    if (prefix == 0) return true;
    std::uint32_t mask = prefix == 32 ? 0xffffffffu : (0xffffffffu << (32 - prefix));
    return (ip.value & mask) == (network.value & mask);
}

std::optional<Cidr> parse_cidr(const std::string& text) {
    auto parts = core::split(text, '/');
    auto ip = parse_ipv4(parts[0]);
    if (!ip) return std::nullopt;
    int prefix = 32;
    if (parts.size() > 1) prefix = std::atoi(parts[1].c_str());
    if (prefix < 0 || prefix > 32) return std::nullopt;
    return Cidr{*ip, static_cast<std::uint8_t>(prefix)};
}

std::string classify_ip(IPv4 ip) {
    auto a = (ip.value >> 24) & 255;
    auto b = (ip.value >> 16) & 255;
    if (a == 10 || (a == 172 && b >= 16 && b <= 31) || (a == 192 && b == 168)) return "private";
    if (a == 127) return "loopback";
    if (a >= 224 && a <= 239) return "multicast";
    if (a == 0 || a >= 240) return "reserved";
    return "public";
}

Flow flow_from_record(const evidence::EvidenceRecord& record) {
    auto get = [&](const std::string& key) -> std::string {
        auto it = record.fields.find(core::normalize_key(key));
        return it == record.fields.end() ? std::string() : it->second;
    };
    Flow f;
    if (auto ip = parse_ipv4(get("src"))) f.src = *ip;
    if (auto ip = parse_ipv4(get("source_ip"))) f.src = *ip;
    if (auto ip = parse_ipv4(get("dst"))) f.dst = *ip;
    if (auto ip = parse_ipv4(get("destination_ip"))) f.dst = *ip;
    f.src_port = static_cast<std::uint16_t>(std::atoi(get("sport").c_str()));
    f.dst_port = static_cast<std::uint16_t>(std::atoi(get("dport").c_str()));
    f.protocol = core::lower(get("proto").empty() ? get("protocol") : get("proto"));
    f.host = get("host");
    f.time = record.timestamp;
    return f;
}

std::vector<Flow> collect_flows(const std::vector<evidence::EvidenceRecord>& records) {
    std::vector<Flow> flows;
    for (const auto& record : records) {
        if (record.kind == evidence::RecordKind::network || record.fields.count("src") || record.fields.count("dst")) {
            flows.push_back(flow_from_record(record));
        }
    }
    return flows;
}

std::vector<std::string> network_findings(const std::vector<Flow>& flows) {
    std::vector<std::string> findings;
    std::map<std::string, int> public_by_host;
    for (const auto& f : flows) {
        auto dst_class = classify_ip(f.dst);
        if (dst_class == "public") public_by_host[f.host]++;
        if (f.dst_port == 3389 && dst_class == "public") findings.push_back("public RDP destination from " + f.host);
        if ((f.dst_port == 22 || f.dst_port == 5985) && classify_ip(f.src) == "public") findings.push_back("administrative inbound flow");
        if (f.protocol.empty()) findings.push_back("flow missing protocol");
    }
    for (const auto& kv : public_by_host) {
        if (kv.second > 20) findings.push_back("host has many public destinations " + kv.first);
    }
    return findings;
}

} // namespace obsidiantrail::network
