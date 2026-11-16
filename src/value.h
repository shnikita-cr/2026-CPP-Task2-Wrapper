#ifndef VALUE_H
#define VALUE_H

#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

// Универсальное значение аргумента / результата: int, double или строка.
class Value {
public:
    using Storage = std::variant<int, double, std::string>;

    Value(int number) : data_(number) {}
    Value(double number) : data_(number) {}
    Value(std::string text) : data_(std::move(text)) {}
    Value(const char* text) : data_(std::string(text)) {}

    // Приведение к типу параметра метода (числа конвертируются друг в друга).
    template <typename T>
    [[nodiscard]] T as() const {
        if constexpr (std::is_arithmetic_v<T>) {
            if (const auto* integer = std::get_if<int>(&data_)) {
                return static_cast<T>(*integer);
            }
            if (const auto* real = std::get_if<double>(&data_)) {
                return static_cast<T>(*real);
            }
        } else {
            if (const auto* text = std::get_if<T>(&data_)) {
                return *text;
            }
        }
        throw std::invalid_argument("Argument has an incompatible type");
    }

    [[nodiscard]] const Storage& storage() const { return data_; }

private:
    Storage data_;
};

inline std::ostream& operator<<(std::ostream& out, const Value& value) {
    std::visit([&out](const auto& item) { out << item; }, value.storage());
    return out;
}

// Именованный аргумент; порядок элементов = порядок параметров метода.
using ArgList = std::vector<std::pair<std::string, Value>>;

#endif // VALUE_H
