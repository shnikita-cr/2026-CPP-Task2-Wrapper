#pragma once

#include <any>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

using ParamMap = std::unordered_map<std::string, std::any>;
using ParamList = std::vector<std::pair<std::string, std::any>>;

namespace detail {

inline std::pair<std::vector<std::string>, ParamMap> prepare_defaults(
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

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
inline void validate_overrides(const ParamMap& default_map, const ParamMap& overrides) {
    for (const auto& entry : overrides) {
        if (default_map.find(entry.first) == default_map.end()) {
            throw std::invalid_argument("Unknown argument: " + entry.first);
        }
    }
}

template <typename TargetType>
std::decay_t<TargetType> extract_value(const std::any& raw_value,
                                       const std::string& param_name) {
    using Decayed = std::decay_t<TargetType>;
    if constexpr (std::is_same_v<Decayed, std::string>) {
        if (const auto* str_ptr = std::any_cast<std::string>(&raw_value); str_ptr != nullptr) {
            return *str_ptr;
        }
        if (const auto* cstr_ptr = std::any_cast<const char*>(&raw_value); cstr_ptr != nullptr) {
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

template <typename TargetType>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::decay_t<TargetType> resolve_arg(const std::string& param_name,
                                     const ParamMap& default_map,
                                     const ParamMap& overrides) {
    if (const auto iter = overrides.find(param_name); iter != overrides.end()) {
        return extract_value<TargetType>(iter->second, param_name);
    }
    return extract_value<TargetType>(default_map.at(param_name), param_name);
}

template <typename Ret,
          typename... Args,
          typename InstancePtr,
          typename MethodPtr,
          std::size_t... Indices>
std::any call_member(InstancePtr instance,
                     MethodPtr method,
                     [[maybe_unused]] const std::vector<std::string>& param_names,
                     const ParamMap& default_map,
                     const ParamMap& overrides,
                     std::index_sequence<Indices...>) {
    validate_overrides(default_map, overrides);
    if constexpr (std::is_void_v<Ret>) {
        (instance->*method)(resolve_arg<Args>(param_names[Indices], default_map, overrides)...);
        return {};
    } else {
        return (instance->*method)(
            resolve_arg<Args>(param_names[Indices], default_map, overrides)...);
    }
}

class ICallable {
public:
    virtual ~ICallable() = default;
    virtual std::any invoke(const ParamMap& overrides) const = 0;
};

template <typename InstancePtr, typename MethodPtr, typename Ret, typename... Args>
class BoundMethod final : public ICallable {
public:
    BoundMethod(InstancePtr instance,
                MethodPtr method,
                std::vector<std::string> param_names,
                ParamMap default_values)
        : instance_(instance),
          method_(method),
          param_names_(std::move(param_names)),
          default_values_(std::move(default_values)) {}

    std::any invoke(const ParamMap& overrides) const override {
        return call_member<Ret, Args...>(instance_,
                                         method_,
                                         param_names_,
                                         default_values_,
                                         overrides,
                                         std::make_index_sequence<sizeof...(Args)>{});
    }

private:
    InstancePtr instance_;
    MethodPtr method_;
    std::vector<std::string> param_names_;
    ParamMap default_values_;
};

template <typename InstancePtr, typename MethodPtr, typename Ret, typename... Args>
std::unique_ptr<ICallable> make_bound_method(InstancePtr instance,
                                             MethodPtr method,
                                             const ParamList& default_params) {
    if (instance == nullptr || method == nullptr) {
        throw std::invalid_argument("Wrapper requires non-null instance and method");
    }
    auto [names, defaults] = prepare_defaults(default_params, sizeof...(Args));
    return std::make_unique<BoundMethod<InstancePtr, MethodPtr, Ret, Args...>>(
        instance, method, std::move(names), std::move(defaults));
}

}  // namespace detail

class Wrapper {
public:
    template <typename Obj, typename Ret, typename... Args>
    Wrapper(Obj* instance, Ret (Obj::*method)(Args...), const ParamList& default_params = {})
        : callable_(detail::make_bound_method<Obj*, Ret (Obj::*)(Args...), Ret, Args...>(
              instance, method, default_params)) {}

    template <typename Obj, typename Ret, typename... Args>
    Wrapper(const Obj* instance,
            Ret (Obj::*method)(Args...) const,
            const ParamList& default_params = {})
        : callable_(
              detail::make_bound_method<const Obj*, Ret (Obj::*)(Args...) const, Ret, Args...>(
                  instance, method, default_params)) {}

    std::any invoke(const ParamMap& overrides = {}) const {
        if (!callable_) {
            throw std::runtime_error("Wrapper has no bound callable");
        }
        return callable_->invoke(overrides);
    }

private:
    std::unique_ptr<detail::ICallable> callable_;
};
