#ifndef WRAPPER_H
#define WRAPPER_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "ArgumentMap.h"
#include "Value.h"

namespace detail {

template <typename Method>
struct MethodTraits;

template <typename Result, typename Class, typename... Args>
struct MethodTraits<Result (Class::*)(Args...)> {
    using ReturnType = Result;
    using ArgumentTuple = std::tuple<Args...>;
};

template <typename Result, typename Class, typename... Args>
struct MethodTraits<Result (Class::*)(Args...) const> : MethodTraits<Result (Class::*)(Args...)> {};

template <typename Traits, std::size_t Index>
using ArgumentType = std::decay_t<std::tuple_element_t<Index, typename Traits::ArgumentTuple>>;

template <typename Traits, typename Subject, typename Method, std::size_t... Indices>
Value callMethod(Subject* subject, Method method, [[maybe_unused]] const std::vector<Value>& values,
                 std::index_sequence<Indices...> /*indices*/) {
    if constexpr (std::is_void_v<typename Traits::ReturnType>) {
        (subject->*method)(values[Indices].template get<ArgumentType<Traits, Indices>>()...);
        return {};
    } else {
        return Value{(subject->*method)(values[Indices].template get<ArgumentType<Traits, Indices>>()...)};
    }
}

}

class Wrapper {
public:
    template <typename Subject, typename Method>
    Wrapper(Subject* subject, Method method, ArgumentMap defaults) : defaults_(std::move(defaults)) {
        using Traits = detail::MethodTraits<Method>;

        static constexpr std::size_t kArity = std::tuple_size_v<typename Traits::ArgumentTuple>;

        if (defaults_.size() != kArity) {
            throw std::invalid_argument("Wrapper: number of default arguments does not match the method");
        }

        invoker_ = [subject, method](const std::vector<Value>& values) {
            return detail::callMethod<Traits>(subject, method, values, std::make_index_sequence<kArity>{});
            };
    }

    [[nodiscard]] Value invoke(const ArgumentMap& overrides) const;

private:
    ArgumentMap defaults_;
    std::function<Value(const std::vector<Value>&)> invoker_;
};

#endif
