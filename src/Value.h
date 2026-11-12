// Значение хранится строкой, чтобы смешивать строки и числа: {{"arg1", 0}, {"arg2", "a"}}.
#pragma once

#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

class Value {
public:
    Value(std::string text) : text_(std::move(text)) {}

    Value(const char* text) : text_(text != nullptr ? text : throw std::invalid_argument("null string")) {}

    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
    Value(T number) : text_(to_text(number)) {}

    [[nodiscard]] const std::string& text() const { return text_; }

    // T: std::string, bool или число. Бросает std::invalid_argument.
    template <typename T>
    [[nodiscard]] T as() const;

private:
    template <typename T>
    static std::string to_text(T number);

    std::string text_;
};

template <typename T>
T Value::as() const {
    if constexpr (std::is_same_v<T, std::string>) {
        return text_;
    } else if constexpr (std::is_same_v<T, bool>) {
        if (text_ == "true" || text_ == "1") {
            return true;
        }
        if (text_ == "false" || text_ == "0") {
            return false;
        }
        throw std::invalid_argument("'" + text_ + "' is not a boolean");
    } else {
        static_assert(std::is_arithmetic_v<T>, "argument type must be std::string, bool or a number");
        std::istringstream stream(text_);
        T number{};
        if (!(stream >> number) || !(stream >> std::ws).eof()) {
            throw std::invalid_argument("'" + text_ + "' is not a number");
        }
        return number;
    }
}

template <typename T>
std::string Value::to_text(T number) {
    if constexpr (std::is_same_v<T, bool>) {
        return number ? "true" : "false";
    } else {
        std::ostringstream stream;
        stream << number;
        return stream.str();
    }
}
