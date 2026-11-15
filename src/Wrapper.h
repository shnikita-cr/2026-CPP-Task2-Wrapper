#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "Value.h"

// i-й аргумент по умолчанию передаётся в i-й параметр метода.
using Arguments = std::vector<std::pair<std::string, Value>>;

class Command {
public:
    virtual ~Command() = default;

    // Бросает std::invalid_argument, если аргумента с таким именем нет
    // или значение не подходит к типу параметра.
    [[nodiscard]] virtual std::string call(const Arguments& overrides) const = 0;
};

template <typename Result, typename... Params>
class Wrapper : public Command {
public:
    template <typename Subject>
    Wrapper(Subject* subject, Result (Subject::*method)(Params...), Arguments defaults)
        : Wrapper(bind(subject, method), std::move(defaults)) {}

    template <typename Subject>
    Wrapper(const Subject* subject, Result (Subject::*method)(Params...) const, Arguments defaults)
        : Wrapper(bind(subject, method), std::move(defaults)) {}

    [[nodiscard]] std::string call(const Arguments& overrides) const override {
        std::vector<Value> values;
        values.reserve(defaults_.size());
        for (const auto& argument : defaults_) {
            values.push_back(argument.second);
        }
        for (const auto& argument : overrides) {
            values[index_of(argument.first)] = argument.second;
        }
        return invoke(values, std::index_sequence_for<Params...>{});
    }

private:
    using Function = std::function<Result(Params...)>;

    Wrapper(Function function, Arguments defaults)
        : function_(std::move(function)), defaults_(std::move(defaults)) {
        if (defaults_.size() != sizeof...(Params)) {
            throw std::invalid_argument("method takes " + std::to_string(sizeof...(Params)) +
                                        " argument(s), but " + std::to_string(defaults_.size()) +
                                        " default value(s) given");
        }
        for (auto it = defaults_.begin(); it != defaults_.end(); ++it) {
            const bool repeated = std::any_of(defaults_.begin(), it, [&it](const auto& argument) {
                return argument.first == it->first;
            });
            if (repeated) {
                throw std::invalid_argument("argument '" + it->first + "' is listed twice");
            }
        }
    }

    template <typename Subject, typename Method>
    static Function bind(Subject* subject, Method method) {
        if (subject == nullptr || method == nullptr) {
            throw std::invalid_argument("object and method must not be null");
        }
        return [subject, method](Params... params) -> Result {
            return (subject->*method)(std::forward<Params>(params)...);
        };
    }

    [[nodiscard]] std::size_t index_of(const std::string& name) const {
        for (std::size_t i = 0; i < defaults_.size(); ++i) {
            if (defaults_[i].first == name) {
                return i;
            }
        }
        throw std::invalid_argument("unknown argument '" + name + "'");
    }

    template <std::size_t... Index>
    [[nodiscard]] std::string invoke([[maybe_unused]] const std::vector<Value>& values,
                                     std::index_sequence<Index...> /*indices*/) const {
        if constexpr (std::is_void_v<Result>) {
            function_(values[Index].template as<std::decay_t<Params>>()...);
            return {};
        } else {
            return Value(function_(values[Index].template as<std::decay_t<Params>>()...)).text();
        }
    }

    Function function_;
    Arguments defaults_;
};
