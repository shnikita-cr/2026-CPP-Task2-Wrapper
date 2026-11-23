#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>
#include "string_utils.h"

template <typename Class, typename ReturnType, typename... Args>
class Wrapper {
public:
    using MethodPtr = ReturnType (*)(Args...);
    using Overrides = std::unordered_map<std::string, std::string>;
    using Executor  = std::function<std::string(const Overrides&)>;

    Wrapper(MethodPtr method, std::tuple<std::string, Args>... defaults)
        : method_(method) {
        (defaults_.emplace_back(std::get<0>(defaults), utils::to_display(std::get<1>(defaults))), ...);
    }

    [[nodiscard]] Executor make_executor() {
        return [this](const Overrides& ov) -> std::string {
            return do_invoke(ov, std::index_sequence_for<Args...>{});
        };
    }

private:
    MethodPtr method_;
    std::vector<std::pair<std::string, std::string>> defaults_;

    template <std::size_t I>
    std::string resolve_argument(const Overrides& ov) {
        const auto& [name, def_str] = defaults_[I];
        auto it = ov.find(name);
        if (it == ov.end()) {
            return def_str;
        }
        return it->second;
    }

    template <std::size_t... I>
    std::string do_invoke(const Overrides& ov, std::index_sequence<I...>) {
        if constexpr (std::is_void_v<ReturnType>) {
            method_(resolve_argument<I>(ov)...);
            return {};
        } else {
            auto result = method_(resolve_argument<I>(ov)...);
            if constexpr (std::is_same_v<ReturnType, std::string>) {
                return result;
            } else {
                return utils::to_display(result);
            }
        }
    }
};

template <typename Class, typename ReturnType, typename... Args, typename... Tuples>
auto make_wrapper(ReturnType (*method)(Args...), Tuples... defaults) {
    return std::make_unique<Wrapper<Class, ReturnType, Args...>>(method, std::move(defaults)...);
}