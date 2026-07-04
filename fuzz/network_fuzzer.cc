#include <string>
#include "obsidiantrail/evidence/record.hpp"
#include "obsidiantrail/network/network.hpp"
#include <cstdint>
#include <cstddef>
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size){std::string s((const char*)data,(const char*)data+size); (void)obsidiantrail::network::parse_ipv4(s); (void)obsidiantrail::network::parse_cidr(s); auto r=obsidiantrail::evidence::parse_record_text(s,"net"); auto f=obsidiantrail::network::collect_flows(r); (void)obsidiantrail::network::network_findings(f); return 0;}
