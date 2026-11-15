#pragma once

#include <any>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Значение одного аргумента команды.
// Хранит копию переданного значения и умеет отдавать его обратно
// в строго заданном типе, который соответствует типу параметра метода.
class Argument {
public:
    Argument() = default;

    template <typename Type>
    Argument(Type value) {
        using Stored = std::decay_t<Type>;
        if constexpr (std::is_same_v<Stored, const char*> || std::is_same_v<Stored, char*>) {
            data_ = std::string(value);
        } else {
            data_ = std::move(value);
        }
    }

    template <typename Type>
    [[nodiscard]] Type as() const {
        if (!data_.has_value()) {
            throw std::runtime_error("argument has no value");
        }
        if (data_.type() != typeid(Type)) {
            throw std::runtime_error("argument has a type that does not match the method parameter");
        }
        return std::any_cast<Type>(data_);
    }

    [[nodiscard]] bool has_value() const {
        return data_.has_value();
    }

private:
    std::any data_;
};

// Аргументы по умолчанию хранятся в порядке параметров метода,
// поэтому позиция элемента соответствует позиции параметра.
using ArgList = std::vector<std::pair<std::string, Argument>>;

// Значения, переданные при вызове, ищутся по имени аргумента.
using ArgMap = std::map<std::string, Argument>;
