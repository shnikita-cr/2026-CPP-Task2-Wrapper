#pragma once

#include <any>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

using ArgMap = std::unordered_map<std::string, std::any>;
using ArgPair = std::pair<std::string, std::any>;

class Wrapper {
public:
    using RawInvoker = std::function<std::any(const std::vector<std::string>&, const ArgMap&)>;
    template <typename Obj, typename Ret, typename... Args>
    Wrapper(Obj* instance, Ret (Obj::*method)(Args...), std::initializer_list<ArgPair> default_args) {
        init(
            instance != nullptr && method != nullptr,
            sizeof...(Args),
            default_args,
            [instance, method](const std::vector<std::string>& names, const ArgMap& args) -> std::any {
                return call_method<Obj, Ret, Args...>(
                    instance, method, names, args, std::index_sequence_for<Args...>{}
                );
            }
        );
    }
    template <typename Obj, typename Ret, typename... Args>
    Wrapper(const Obj* instance, Ret (Obj::*method)(Args...) const, std::initializer_list<ArgPair> default_args) {
        init(
            instance != nullptr && method != nullptr,
            sizeof...(Args),
            default_args,
            [instance, method](const std::vector<std::string>& names, const ArgMap& args) -> std::any {
                return call_const_method<Obj, Ret, Args...>(
                    instance, method, names, args, std::index_sequence_for<Args...>{}
                );
            }
        );
    }
    std::any execute(const ArgMap& args = {}) {
        ArgMap final_args = default_args_;
        for (const auto& [name, val] : args) {
            auto it = final_args.find(name);
            if (it == final_args.end()) {
                throw std::invalid_argument("Unknown argument name: " + name);
            }
            it->second = val;
        }
        return invoker_(param_names_, final_args);
    }
private:
    std::vector<std::string> param_names_;
    ArgMap default_args_;
    RawInvoker invoker_;
    void init(
        bool valid_ptrs,
        std::size_t expected_args,
        std::initializer_list<ArgPair> defaults,
        RawInvoker invoker
    ) {
        if (!valid_ptrs) {
            throw std::invalid_argument("Instance and method pointers must not be null");
        }
        if (defaults.size() != expected_args) {
            throw std::invalid_argument("Number of default arguments must match method signature");
        }

        param_names_.reserve(defaults.size());
        for (const auto& [name, val] : defaults) {
            param_names_.push_back(name);
            default_args_[name] = val;
        }
        invoker_ = std::move(invoker);
    }
    template <typename T>
    static std::decay_t<T> cast_arg(const std::any& val, const std::string& name) {
        using CleanT = std::decay_t<T>;
        try {
            if constexpr (std::is_same_v<CleanT, std::string>) {
                if (val.type() == typeid(const char*)) {
                    return std::string(std::any_cast<const char*>(val));
                }
            }
            return std::any_cast<CleanT>(val);
        } catch (const std::bad_any_cast&) {
            throw std::invalid_argument("Type mismatch for argument: " + name);
        }
    }
    template <typename Obj, typename Ret, typename... Args, std::size_t... Is>
    static std::any call_method(
        Obj* instance,
        Ret (Obj::*method)(Args...),
        const std::vector<std::string>& names,
        const ArgMap& args,
        std::index_sequence<Is...> 
    ) {
        if constexpr (std::is_void_v<Ret>) {
            (instance->*method)(cast_arg<Args>(args.at(names[Is]), names[Is])...);
            return {};
        } else {
            return (instance->*method)(cast_arg<Args>(args.at(names[Is]), names[Is])...);
        }
    }
    template <typename Obj, typename Ret, typename... Args, std::size_t... Is>
    static std::any call_const_method(
        const Obj* instance,
        Ret (Obj::*method)(Args...) const,
        const std::vector<std::string>& names,
        const ArgMap& args,
        std::index_sequence<Is...> 
    ) {
        if constexpr (std::is_void_v<Ret>) {
            (instance->*method)(cast_arg<Args>(args.at(names[Is]), names[Is])...);
            return {};
        } else {
            return (instance->*method)(cast_arg<Args>(args.at(names[Is]), names[Is])...);
        }
    }
};
