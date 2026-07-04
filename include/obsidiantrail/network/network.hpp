#ifndef OBSIDIANTRAIL_NETWORK_NETWORK_HPP
#define OBSIDIANTRAIL_NETWORK_NETWORK_HPP

#include "obsidiantrail/evidence/record.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace obsidiantrail::network {

struct IPv4 {
    std::uint32_t value = 0;
    bool operator<(const IPv4& other) const { return value < other.value; }
};

struct Cidr {
    IPv4 network;
    std::uint8_t prefix = 32;
    bool contains(IPv4 ip) const;
};

struct Flow {
    IPv4 src;
    IPv4 dst;
    std::uint16_t src_port = 0;
    std::uint16_t dst_port = 0;
    std::string protocol;
    std::string host;
    core::Timestamp time;
};

std::optional<IPv4> parse_ipv4(const std::string& text);
std::optional<Cidr> parse_cidr(const std::string& text);
std::string ipv4_to_string(IPv4 ip);
std::string classify_ip(IPv4 ip);
Flow flow_from_record(const evidence::EvidenceRecord& record);
std::vector<Flow> collect_flows(const std::vector<evidence::EvidenceRecord>& records);
std::vector<std::string> network_findings(const std::vector<Flow>& flows);

} // namespace obsidiantrail::network

#endif
