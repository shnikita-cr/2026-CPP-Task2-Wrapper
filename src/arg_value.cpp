#include "arg_value.h"
#include <stdexcept>
#include <utility>

namespace {

bool is_truthy(const std::string& str_val) noexcept {
    return str_val == "true" || str_val == "1" || str_val == "найдено" || str_val == "yes";
}

} // namespace

ArgValue::ArgValue() = default;

ArgValue::ArgValue(int val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(long val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(double val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(float val) : value_(val), str_repr_(std::to_string(val)) {}

ArgValue::ArgValue(bool val) : value_(val), str_repr_(val ? "true" : "false") {}

ArgValue::ArgValue(const char* val)
    : value_(val != nullptr ? std::string(val) : ""),
      str_repr_(val != nullptr ? val : "") {}

ArgValue::ArgValue(const std::string& val) : value_(val), str_repr_(val) {}

ArgValue::ArgValue(std::string&& val) noexcept
    : value_(val), str_repr_(std::move(val)) {}

const std::string& ArgValue::to_string() const noexcept {
    return str_repr_;
}

bool ArgValue::has_value() const noexcept {
    return value_.has_value() || !str_repr_.empty();
}

const std::any& ArgValue::raw_any() const noexcept {
    return value_;
}

int ArgValue::as_int() const {
    if (value_.type() == typeid(int)) {
        return std::any_cast<int>(value_);
    }
    if (value_.type() == typeid(long)) {
        return static_cast<int>(std::any_cast<long>(value_));
    }
    if (value_.type() == typeid(double)) {
        return static_cast<int>(std::any_cast<double>(value_));
    }
    if (value_.type() == typeid(bool)) {
        return std::any_cast<bool>(value_) ? 1 : 0;
    }
    if (!str_repr_.empty()) {
        return std::stoi(str_repr_);
    }
    return 0;
}

long ArgValue::as_long() const {
    if (value_.type() == typeid(long)) {
        return std::any_cast<long>(value_);
    }
    if (value_.type() == typeid(int)) {
        return static_cast<long>(std::any_cast<int>(value_));
    }
    return !str_repr_.empty() ? std::stol(str_repr_) : 0L;
}

double ArgValue::as_double() const {
    if (value_.type() == typeid(double)) {
        return std::any_cast<double>(value_);
    }
    if (value_.type() == typeid(float)) {
        return static_cast<double>(std::any_cast<float>(value_));
    }
    if (value_.type() == typeid(int)) {
        return static_cast<double>(std::any_cast<int>(value_));
    }
    return !str_repr_.empty() ? std::stod(str_repr_) : 0.0;
}

float ArgValue::as_float() const {
    if (value_.type() == typeid(float)) {
        return std::any_cast<float>(value_);
    }
    if (value_.type() == typeid(double)) {
        return static_cast<float>(std::any_cast<double>(value_));
    }
    if (value_.type() == typeid(int)) {
        return static_cast<float>(std::any_cast<int>(value_));
    }
    return !str_repr_.empty() ? std::stof(str_repr_) : 0.0F;
}

bool ArgValue::as_bool() const {
    if (value_.type() == typeid(bool)) {
        return std::any_cast<bool>(value_);
    }
    if (value_.type() == typeid(int)) {
        return std::any_cast<int>(value_) != 0;
    }
    return is_truthy(str_repr_);
}

ArgsMap::ArgsMap(std::initializer_list<std::pair<std::string, ArgValue>> init)
    : entries_(init) {}

ArgsMap::ArgsMap(const std::vector<std::pair<std::string, ArgValue>>& entries)
    : entries_(entries) {}

ArgsMap::ArgsMap(const std::map<std::string, ArgValue>& map) {
    entries_.reserve(map.size());
    for (const auto& entry : map) {
        entries_.emplace_back(entry.first, entry.second);
    }
}

ArgsMap::ArgsMap(const std::map<std::string, std::string>& map) {
    entries_.reserve(map.size());
    for (const auto& entry : map) {
        entries_.emplace_back(entry.first, ArgValue(entry.second));
    }
}

ArgsMap::ArgsMap(const std::unordered_map<std::string, ArgValue>& map) {
    entries_.reserve(map.size());
    for (const auto& entry : map) {
        entries_.emplace_back(entry.first, entry.second);
    }
}

ArgsMap::ArgsMap(const std::unordered_map<std::string, std::string>& map) {
    entries_.reserve(map.size());
    for (const auto& entry : map) {
        entries_.emplace_back(entry.first, ArgValue(entry.second));
    }
}

const ArgValue* ArgsMap::find(const std::string& name) const noexcept {
    for (const auto& entry : entries_) {
        if (entry.first == name) {
            return &entry.second;
        }
    }
    return nullptr;
}

bool ArgsMap::has(const std::string& name) const noexcept {
    return find(name) != nullptr;
}

const ArgValue& ArgsMap::at(const std::string& name) const {
    const ArgValue* val = find(name);
    if (val == nullptr) {
        throw std::out_of_range("Argument not found: " + name);
    }
    return *val;
}

std::size_t ArgsMap::size() const noexcept {
    return entries_.size();
}

bool ArgsMap::empty() const noexcept {
    return entries_.empty();
}

const std::vector<std::pair<std::string, ArgValue>>& ArgsMap::entries() const noexcept {
    return entries_;
}
