#ifndef OBSIDIANTRAIL_CORE_JSONLITE_HPP
#define OBSIDIANTRAIL_CORE_JSONLITE_HPP

#include "obsidiantrail/core/result.hpp"
#include <map>
#include <string>
#include <vector>

namespace obsidiantrail::core {

enum class JsonKind { null_value, boolean, number, string, array, object };

struct JsonValue {
    JsonKind kind = JsonKind::null_value;
    bool boolean = false;
    double number = 0.0;
    std::string string;
    std::vector<JsonValue> array;
    std::map<std::string, JsonValue> object;
};

Result<JsonValue> parse_jsonlite(const std::string& text);
std::string render_jsonlite(const JsonValue& value);
std::vector<std::string> flatten_json_paths(const JsonValue& value);
std::map<std::string, std::string> object_to_pairs(const JsonValue& value);
std::string json_kind_name(JsonKind kind);

} // namespace obsidiantrail::core

#endif
