#pragma once

#include <cstddef>

template <class T>
struct remove_ref
{
    using type = T;
};

template <class T>
struct remove_ref<T&>
{
    using type = T;
};
template <class T>
struct remove_ref<T&&>
{
    using type = T;
};

template <class T>
struct remove_cv
{
    using type = T;
};
template <class T>
struct remove_cv<const T>
{
    using type = T;
};
template <class T>
struct remove_cv<volatile T>
{
    using type = T;
};
template <class T>
struct remove_cv<const volatile T>
{
    using type = T;
};

template <class T>
struct decay_impl
{
    using type = typename remove_cv<T>::type;
};

template <class T, std::size_t N>
struct decay_impl<T[N]>// NOLINT(modernize-avoid-c-arrays)
{
    using type = T*;
};

template <class T>
struct decay_impl<T[]>// NOLINT(modernize-avoid-c-arrays)
{
    using type = T*;
};

template <class R, class... A>
struct decay_impl<R(A...)>
{
    using type = R(*)(A...);
};
template <class R, class... A>
struct decay_impl<R(A..., ...)>
{
    using type = R(*)(A..., ...);
};

template <class T>
struct decay
{
    using type = typename decay_impl<typename remove_ref<T>::type>::type;
};

template <class T>
using decay_t = typename decay<T>::type;