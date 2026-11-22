#pragma once

#include <any>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

using ParamMap = std::unordered_map<std::string, std::any>;
using ParamList = std::vector<std::pair<std::string, std::any>>;

class Wrapper {
public:
    template <typename Obj, typename Ret, typename... Args>
    Wrapper(Obj* instance, Ret (Obj::*method)(Args...), const ParamList& default_params = {}) {
        if (instance == nullptr || method == nullptr) {
            throw std::invalid_argument("Wrapper requires non-null instance and method");
        }
        auto [names, defaults] = prepare_defaults(default_params, sizeof...(Args));
        invoker_ = make_invoker<Obj*, Ret (Obj::*)(Args...), Ret, Args...>(
            instance, method, std::move(names), std::move(defaults));
    }

    template <typename Obj, typename Ret, typename... Args>
    Wrapper(const Obj* instance,
            Ret (Obj::*method)(Args...) const,
            const ParamList& default_params = {}) {
        if (instance == nullptr || method == nullptr) {
            throw std::invalid_argument("Wrapper requires non-null instance and method");
        }
        auto [names, defaults] = prepare_defaults(default_params, sizeof...(Args));
        invoker_ = make_invoker<const Obj*, Ret (Obj::*)(Args...) const, Ret, Args...>(
            instance, method, std::move(names), std::move(defaults));
    }

    std::any invoke(const ParamMap& overrides = {}) const {
        if (!invoker_) {
            throw std::runtime_error("Wrapper has no bound callable");
        }
        return invoker_(overrides);
    }

private:
    static std::pair<std::vector<std::string>, ParamMap> prepare_defaults(
        const ParamList& default_params,
        std::size_t expected_count) {
        if (default_params.size() != expected_count) {
            throw std::invalid_argument("Default argument count does not match method arity");
        }

        std::vector<std::string> names;
        names.reserve(default_params.size());
        ParamMap default_map;
        default_map.reserve(default_params.size());

        for (const auto& entry : default_params) {
            if (entry.first.empty()) {
                throw std::invalid_argument("Argument name must not be empty");
            }
            if (!default_map.emplace(entry.first, entry.second).second) {
                throw std::invalid_argument("Duplicate argument name: " + entry.first);
            }
            names.push_back(entry.first);
        }

        return {std::move(names), std::move(default_map)};
    }

    static void validate_overrides(const ParamMap& default_map, const ParamMap& overrides) {
        for (const auto& entry : overrides) {
            if (default_map.find(entry.first) == default_map.end()) {
                throw std::invalid_argument("Unknown argument: " + entry.first);
            }
        }
    }

    template <typename T>
    static std::decay_t<T> extract_value(const std::any& raw_value,
                                         const std::string& param_name) {
        using Decayed = std::decay_t<T>;
        if constexpr (std::is_same_v<Decayed, std::string>) {
            if (const auto* str_ptr = std::any_cast<std::string>(&raw_value);
                str_ptr != nullptr) {
                return *str_ptr;
            }
            if (const auto* cstr_ptr = std::any_cast<const char*>(&raw_value);
                cstr_ptr != nullptr) {
                if (*cstr_ptr == nullptr) {
                    throw std::invalid_argument("Null string pointer for argument: " + param_name);
                }
                return std::string(*cstr_ptr);
            }
            throw std::invalid_argument("Type mismatch for string argument: " + param_name);
        } else {
            if (const auto* val_ptr = std::any_cast<Decayed>(&raw_value); val_ptr != nullptr) {
                return *val_ptr;
            }
            throw std::invalid_argument("Type mismatch for argument: " + param_name);
        }
    }

    template <typename T>
    static std::decay_t<T> resolve_arg(const std::string& param_name,
                                       const ParamMap& default_map,
                                       const ParamMap& overrides) {
        if (const auto iter = overrides.find(param_name); iter != overrides.end()) {
            return extract_value<T>(iter->second, param_name);
        }
        return extract_value<T>(default_map.at(param_name), param_name);
    }

    template <typename Ret,
              typename... Args,
              typename Ptr,
              typename Method,
              std::size_t... Indices>
    static std::any invoke_with_args(Ptr instance,
                                     Method method,
                                     [[maybe_unused]] const std::vector<std::string>& names,
                                     const ParamMap& default_map,
                                     const ParamMap& overrides,
                                     std::index_sequence<Indices...>) {
        validate_overrides(default_map, overrides);
        if constexpr (std::is_void_v<Ret>) {
            (instance->*method)(resolve_arg<Args>(names[Indices], default_map, overrides)...);
            return {};
        } else {
            return (instance->*method)(
                resolve_arg<Args>(names[Indices], default_map, overrides)...);
        }
    }

    template <typename Ptr, typename Method, typename Ret, typename... Args>
    static std::function<std::any(const ParamMap&)> make_invoker(
        Ptr instance,
        Method method,
        std::vector<std::string> param_names,
        ParamMap defaults) {
        return [instance,
                method,
                names = std::move(param_names),
                default_map = std::move(defaults)](const ParamMap& overrides) -> std::any {
            return invoke_with_args<Ret, Args...>(
                instance,
                method,
                names,
                default_map,
                overrides,
                std::make_index_sequence<sizeof...(Args)>{});
        };
    }

    std::function<std::any(const ParamMap&)> invoker_;
};
