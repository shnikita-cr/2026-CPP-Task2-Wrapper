#ifndef WRAPPER_H
#define WRAPPER_H

#include "Arguments.h"

#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <cstddef>

template <typename Method>
struct MethodTraits;

template <typename Class, typename Return, typename... Args>
struct MethodTraits<Return (Class::*)(Args...)> {
    using ReturnType = Return;
    using ArgsTuple = std::tuple<Args...>;

    static constexpr std::size_t arity = sizeof...(Args);
};

template <typename Class, typename Return, typename... Args>
struct MethodTraits<Return (Class::*)(Args...) const> {
    using ReturnType = Return;
    using ArgsTuple = std::tuple<Args...>;

    static constexpr std::size_t arity = sizeof...(Args);
};

class Wrapper {
public:
    template <typename Object, typename Method>
    Wrapper(Object& object, Method method, Arguments defaults = {})
        : defaults_(std::move(defaults)) {
        invoker_ = [&object, method](const Arguments& arguments) {
            using Traits = MethodTraits<Method>;
            using Tuple = typename Traits::ArgsTuple;

            return invokeMethod(
                object,
                method,
                arguments,
                static_cast<Tuple*>(nullptr),
                std::make_index_sequence<Traits::arity>{}
            );
        };
    }

    std::string execute(const Arguments& overrides = {}) const {
        Arguments arguments = defaults_;

        for (const auto& item : overrides) {
            arguments[item.first] = item.second;
        }

        return invoker_(arguments);
    }

private:
    Arguments defaults_;

    std::function<std::string(const Arguments&)> invoker_;

    template <typename Argument>
    static decltype(auto) getArgument(
        const Arguments& arguments,
        const std::string& key
    ) {
        using Value = std::remove_reference_t<Argument>;

        if constexpr (std::is_lvalue_reference_v<Argument>) {
            return std::any_cast<Value&>(arguments.at(key));
        } else {
            return std::any_cast<std::decay_t<Argument>>(
                arguments.at(key)
            );
        }
    }

    template <
        typename Object,
        typename Method,
        typename Tuple,
        std::size_t... Index
    >
    static std::string invokeMethod(
        Object& object,
        Method method,
        const Arguments& arguments,
        Tuple*,
        std::index_sequence<Index...>
    ) {
        using Return = std::invoke_result_t<
            Method,
            Object&,
            std::tuple_element_t<Index, Tuple>...
        >;

        if constexpr (std::is_void_v<Return>) {
            std::invoke(
                method,
                object,
                getArgument<std::tuple_element_t<Index, Tuple>>(
                    arguments,
                    "arg" + std::to_string(Index)
                )...
            );

            return "OK";
        } else if constexpr (
            std::is_same_v<std::decay_t<Return>, std::string>
        ) {
            return std::invoke(
                method,
                object,
                getArgument<std::tuple_element_t<Index, Tuple>>(
                    arguments,
                    "arg" + std::to_string(Index)
                )...
            );
        } else {
            std::ostringstream output;

            output << std::invoke(
                method,
                object,
                getArgument<std::tuple_element_t<Index, Tuple>>(
                    arguments,
                    "arg" + std::to_string(Index)
                )...
            );

            return output.str();
        }
    }
};

#endif // WRAPPER_H
