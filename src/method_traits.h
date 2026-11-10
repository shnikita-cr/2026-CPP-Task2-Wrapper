#ifndef WRAPPER_METHOD_TRAITS_H
#define WRAPPER_METHOD_TRAITS_H

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace detail
{

template <typename T>
inline constexpr bool supported_parameter =
    std::is_copy_constructible_v<std::decay_t<T>> &&
    (!std::is_reference_v<T> ||
     (std::is_lvalue_reference_v<T> && std::is_const_v<std::remove_reference_t<T>>)) &&
    !std::is_same_v<std::decay_t<T>, char*>;

template <typename Result, typename... Args> struct MethodSignature
{
    using ReturnType = Result;
    using Parameters = std::tuple<Args...>;

    static constexpr std::size_t arity                = sizeof...(Args);
    static constexpr bool        parameters_supported = (supported_parameter<Args> && ...);

    template <typename Object, typename Method>
    static constexpr bool accepts_object =
        std::is_invocable_v<Method, Object*, const std::decay_t<Args>&...>;
};

template <typename Method> struct MethodTraits : MethodSignature<void>
{
    static constexpr bool supported = false;
};

template <typename Object, typename Result, typename... Args>
struct MethodTraits<Result (Object::*)(Args...)> : MethodSignature<Result, Args...>
{
    static constexpr bool supported = true;
};

template <typename Object, typename Result, typename... Args>
struct MethodTraits<Result (Object::*)(Args...) const> : MethodTraits<Result (Object::*)(Args...)>
{
};

template <typename Object, typename Result, typename... Args>
struct MethodTraits<Result (Object::*)(Args...) noexcept>
    : MethodTraits<Result (Object::*)(Args...)>
{
};

template <typename Object, typename Result, typename... Args>
struct MethodTraits<Result (Object::*)(Args...) const noexcept>
    : MethodTraits<Result (Object::*)(Args...)>
{
};

} // namespace detail

#endif // WRAPPER_METHOD_TRAITS_H
