#pragma once
#include <any>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

class Argument {
public:
    Argument(const char* value) {
        if (value == nullptr) {
            throw std::invalid_argument("Null string argument");
        }
        value_ = std::string(value);
    }

    template <typename T,
              std::enable_if_t<!std::is_convertible_v<T, const char*> &&
                               !std::is_same_v<std::decay_t<T>, Argument>, int> = 0>
    Argument(T&& value) : value_(std::forward<T>(value)) {}

    template <typename T>
    [[nodiscard]] std::decay_t<T> get() const {
        const auto* value = std::any_cast<std::decay_t<T>>(&value_);
        if (value == nullptr) {
            throw std::invalid_argument("Wrong argument type");
        }
        return *value;
    }

private:
    std::any value_;
};
