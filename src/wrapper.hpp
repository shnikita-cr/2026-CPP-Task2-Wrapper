#pragma once

#include "argument.hpp"

#include <cstddef>
#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Инкапсулирует объект, его метод и аргументы по умолчанию.
// Вызов метода выполняется через Engine по строковому имени команды,
// значения аргументов при этом можно частично переопределить.
class Wrapper {
public:
    template <typename Object, typename Result, typename... Args>
    Wrapper(Object* object, Result (Object::*method)(Args...), ArgList defaults)
        : defaults_(std::move(defaults)) {
        if (object == nullptr) {
            throw std::runtime_error("wrapper object is null");
        }
        if (defaults_.size() != sizeof...(Args)) {
            throw std::runtime_error("argument count does not match the method signature");
        }
        const std::vector<std::string> names = collect_names(defaults_);
        call_ = [object, method, names](const ArgMap& args) {
            return invoke(object, method, names, args, std::index_sequence_for<Args...>{});
        };
    }

    template <typename Object, typename Result, typename... Args>
    Wrapper(const Object* object, Result (Object::*method)(Args...) const, ArgList defaults)
        : defaults_(std::move(defaults)) {
        if (object == nullptr) {
            throw std::runtime_error("wrapper object is null");
        }
        if (defaults_.size() != sizeof...(Args)) {
            throw std::runtime_error("argument count does not match the method signature");
        }
        const std::vector<std::string> names = collect_names(defaults_);
        call_ = [object, method, names](const ArgMap& args) {
            return invoke(object, method, names, args, std::index_sequence_for<Args...>{});
        };
    }

    [[nodiscard]] std::string execute(const ArgMap& overrides) const {
        ArgMap merged = merge_arguments(overrides);
        return call_(merged);
    }

private:
    ArgList defaults_;
    std::function<std::string(const ArgMap&)> call_;

    static std::vector<std::string> collect_names(const ArgList& defaults) {
        std::vector<std::string> names;
        names.reserve(defaults.size());
        for (const auto& entry : defaults) {
            names.push_back(entry.first);
        }
        return names;
    }

    [[nodiscard]] ArgMap merge_arguments(const ArgMap& overrides) const {
        ArgMap merged;
        for (const auto& entry : defaults_) {
            merged.emplace(entry.first, entry.second);
        }
        for (const auto& entry : overrides) {
            if (merged.find(entry.first) == merged.end()) {
                throw std::runtime_error("unknown argument: " + entry.first);
            }
            merged[entry.first] = entry.second;
        }
        return merged;
    }

    template <typename Result>
    static std::string to_string_result(Result&& result) {
        using Value = std::decay_t<Result>;
        if constexpr (std::is_same_v<Value, std::string>) {
            return std::forward<Result>(result);
        } else {
            std::ostringstream stream;
            stream << result;
            return stream.str();
        }
    }

    template <typename Object, typename Result, typename... Args, std::size_t... Index>
    static std::string invoke(Object* object,
                              Result (Object::*method)(Args...),
                              const std::vector<std::string>& names,
                              const ArgMap& args,
                              std::index_sequence<Index...> /*unused*/) {
        if constexpr (std::is_void_v<Result>) {
            (object->*method)(args.at(names[Index]).template as<std::decay_t<Args>>()...);
            return {};
        } else {
            return to_string_result((object->*method)(args.at(names[Index]).template as<std::decay_t<Args>>()...));
        }
    }

    template <typename Object, typename Result, typename... Args, std::size_t... Index>
    static std::string invoke(const Object* object,
                              Result (Object::*method)(Args...) const,
                              const std::vector<std::string>& names,
                              const ArgMap& args,
                              std::index_sequence<Index...> /*unused*/) {
        if constexpr (std::is_void_v<Result>) {
            (object->*method)(args.at(names[Index]).template as<std::decay_t<Args>>()...);
            return {};
        } else {
            return to_string_result((object->*method)(args.at(names[Index]).template as<std::decay_t<Args>>()...));
        }
    }
};
