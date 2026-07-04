#include "obsidiantrail/core/jsonlite.hpp"
#include "obsidiantrail/core/text.hpp"
#include <cstdlib>
#include <sstream>

namespace obsidiantrail::core {
namespace {

class Parser {
public:
    explicit Parser(const std::string& input) : input_(input) {}
    Result<JsonValue> parse() {
        skip();
        auto value = parse_value();
        if (!value) return value;
        skip();
        if (pos_ != input_.size()) return Result<JsonValue>::err("json.trailing", "trailing text after JSON value");
        return value;
    }
private:
    void skip() {
        while (pos_ < input_.size() && (input_[pos_] == ' ' || input_[pos_] == '\n' || input_[pos_] == '\r' || input_[pos_] == '\t')) ++pos_;
    }
    bool consume(char ch) {
        skip();
        if (pos_ < input_.size() && input_[pos_] == ch) { ++pos_; return true; }
        return false;
    }
    Result<JsonValue> parse_value() {
        skip();
        if (pos_ >= input_.size()) return Result<JsonValue>::err("json.eof", "unexpected end of input");
        char ch = input_[pos_];
        if (ch == '"') return parse_string_value();
        if (ch == '{') return parse_object();
        if (ch == '[') return parse_array();
        if (ch == '-' || (ch >= '0' && ch <= '9')) return parse_number();
        if (input_.compare(pos_, 4, "true") == 0) { pos_ += 4; JsonValue v; v.kind = JsonKind::boolean; v.boolean = true; return Result<JsonValue>::ok(v); }
        if (input_.compare(pos_, 5, "false") == 0) { pos_ += 5; JsonValue v; v.kind = JsonKind::boolean; return Result<JsonValue>::ok(v); }
        if (input_.compare(pos_, 4, "null") == 0) { pos_ += 4; return Result<JsonValue>::ok(JsonValue{}); }
        return Result<JsonValue>::err("json.token", "unexpected token");
    }
    Result<std::string> parse_string() {
        if (!consume('"')) return Result<std::string>::err("json.string", "expected string");
        std::string out;
        while (pos_ < input_.size()) {
            char ch = input_[pos_++];
            if (ch == '"') return Result<std::string>::ok(out);
            if (ch == '\\') {
                if (pos_ >= input_.size()) return Result<std::string>::err("json.escape", "unterminated escape");
                char esc = input_[pos_++];
                if (esc == 'n') out.push_back('\n');
                else if (esc == 'r') out.push_back('\r');
                else if (esc == 't') out.push_back('\t');
                else out.push_back(esc);
            } else {
                out.push_back(ch);
            }
        }
        return Result<std::string>::err("json.string.eof", "unterminated string");
    }
    Result<JsonValue> parse_string_value() {
        auto s = parse_string();
        if (!s) return Result<JsonValue>::err(s.code(), s.message());
        JsonValue v;
        v.kind = JsonKind::string;
        v.string = s.value();
        return Result<JsonValue>::ok(v);
    }
    Result<JsonValue> parse_number() {
        std::size_t start = pos_;
        if (input_[pos_] == '-') ++pos_;
        while (pos_ < input_.size() && input_[pos_] >= '0' && input_[pos_] <= '9') ++pos_;
        if (pos_ < input_.size() && input_[pos_] == '.') {
            ++pos_;
            while (pos_ < input_.size() && input_[pos_] >= '0' && input_[pos_] <= '9') ++pos_;
        }
        JsonValue v;
        v.kind = JsonKind::number;
        v.number = std::strtod(input_.substr(start, pos_ - start).c_str(), nullptr);
        return Result<JsonValue>::ok(v);
    }
    Result<JsonValue> parse_array() {
        consume('[');
        JsonValue v;
        v.kind = JsonKind::array;
        skip();
        if (consume(']')) return Result<JsonValue>::ok(v);
        for (;;) {
            auto child = parse_value();
            if (!child) return child;
            v.array.push_back(child.value());
            if (consume(']')) return Result<JsonValue>::ok(v);
            if (!consume(',')) return Result<JsonValue>::err("json.array", "expected comma in array");
        }
    }
    Result<JsonValue> parse_object() {
        consume('{');
        JsonValue v;
        v.kind = JsonKind::object;
        skip();
        if (consume('}')) return Result<JsonValue>::ok(v);
        for (;;) {
            auto key = parse_string();
            if (!key) return Result<JsonValue>::err(key.code(), key.message());
            if (!consume(':')) return Result<JsonValue>::err("json.object", "expected object colon");
            auto child = parse_value();
            if (!child) return child;
            v.object[key.value()] = child.value();
            if (consume('}')) return Result<JsonValue>::ok(v);
            if (!consume(',')) return Result<JsonValue>::err("json.object", "expected comma in object");
        }
    }
    const std::string& input_;
    std::size_t pos_ = 0;
};

void flatten_into(const JsonValue& value, const std::string& prefix, std::vector<std::string>& out) {
    if (value.kind == JsonKind::object) {
        for (const auto& kv : value.object) flatten_into(kv.second, prefix.empty() ? kv.first : prefix + "." + kv.first, out);
    } else if (value.kind == JsonKind::array) {
        for (std::size_t i = 0; i < value.array.size(); ++i) flatten_into(value.array[i], prefix + "[" + std::to_string(i) + "]", out);
    } else {
        out.push_back(prefix);
    }
}

} // namespace

Result<JsonValue> parse_jsonlite(const std::string& text) {
    return Parser(text).parse();
}

std::string json_kind_name(JsonKind kind) {
    switch (kind) {
        case JsonKind::null_value: return "null";
        case JsonKind::boolean: return "boolean";
        case JsonKind::number: return "number";
        case JsonKind::string: return "string";
        case JsonKind::array: return "array";
        case JsonKind::object: return "object";
    }
    return "unknown";
}

std::string render_jsonlite(const JsonValue& value) {
    std::ostringstream out;
    switch (value.kind) {
        case JsonKind::null_value: out << "null"; break;
        case JsonKind::boolean: out << (value.boolean ? "true" : "false"); break;
        case JsonKind::number: out << value.number; break;
        case JsonKind::string: out << '"' << value.string << '"'; break;
        case JsonKind::array:
            out << "[";
            for (std::size_t i = 0; i < value.array.size(); ++i) { if (i) out << ","; out << render_jsonlite(value.array[i]); }
            out << "]";
            break;
        case JsonKind::object:
            out << "{";
            for (auto it = value.object.begin(); it != value.object.end(); ++it) {
                if (it != value.object.begin()) out << ",";
                out << '"' << it->first << "\":" << render_jsonlite(it->second);
            }
            out << "}";
            break;
    }
    return out.str();
}

std::vector<std::string> flatten_json_paths(const JsonValue& value) {
    std::vector<std::string> out;
    flatten_into(value, "", out);
    return out;
}

std::map<std::string, std::string> object_to_pairs(const JsonValue& value) {
    std::map<std::string, std::string> out;
    if (value.kind != JsonKind::object) return out;
    for (const auto& kv : value.object) {
        if (kv.second.kind == JsonKind::string) out[normalize_key(kv.first)] = kv.second.string;
        else if (kv.second.kind == JsonKind::number) out[normalize_key(kv.first)] = std::to_string(kv.second.number);
        else if (kv.second.kind == JsonKind::boolean) out[normalize_key(kv.first)] = kv.second.boolean ? "true" : "false";
        else out[normalize_key(kv.first)] = json_kind_name(kv.second.kind);
    }
    return out;
}

} // namespace obsidiantrail::core
