#ifndef OBSIDIANTRAIL_CORE_RESULT_HPP
#define OBSIDIANTRAIL_CORE_RESULT_HPP

#include <string>
#include <utility>

namespace obsidiantrail::core {

enum class Severity { info, warning, error };

template <typename T>
class Result {
public:
    static Result ok(T value) { return Result(std::move(value)); }
    static Result err(std::string code, std::string message) {
        Result r;
        r.ok_ = false;
        r.code_ = std::move(code);
        r.message_ = std::move(message);
        return r;
    }
    bool ok() const { return ok_; }
    explicit operator bool() const { return ok_; }
    const T& value() const { return value_; }
    T& value() { return value_; }
    const std::string& code() const { return code_; }
    const std::string& message() const { return message_; }
private:
    Result() = default;
    explicit Result(T value) : ok_(true), value_(std::move(value)) {}
    bool ok_ = false;
    T value_{};
    std::string code_;
    std::string message_;
};

} // namespace obsidiantrail::core

#endif
