#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "arg_converter.h"

class WrapperBase {
public:
    virtual ~WrapperBase() = default;
    [[nodiscard]] virtual std::string execute(
        const std::map<std::string, std::string>& overrides) = 0;
};

template <typename TObj, typename TRet, typename... TArgs>
class Wrapper : public WrapperBase {
public:
    using MethodPtr = TRet(TObj::*)(TArgs...);

    Wrapper(TObj* obj, MethodPtr method,
        std::vector<std::string> arg_names,
        std::map<std::string, std::string> defaults)
        : m_obj(obj),
        m_method(method),
        m_arg_names(std::move(arg_names)),
        m_defaults(std::move(defaults)) {
        if (m_arg_names.size() != sizeof...(TArgs)) {
            throw std::invalid_argument("Arg names count mismatch");
        }
    }

    [[nodiscard]] std::string execute(
        const std::map<std::string, std::string>& overrides) override {
        std::map<std::string, std::string> merged = m_defaults;
        for (const auto& kv : overrides) {
            merged[kv.first] = kv.second;
        }
        auto args = build_args(merged, std::index_sequence_for<TArgs...>{});
        if constexpr (std::is_void_v<TRet>) {
            std::apply(m_method, std::tuple_cat(std::make_tuple(m_obj), args));
            return {};
        }
        else {
            TRet result = std::apply(
                m_method, std::tuple_cat(std::make_tuple(m_obj), args));
            return ArgConverter<TRet>::to_string(result);
        }
    }

private:
    template <std::size_t... Is>
    auto build_args(const std::map<std::string, std::string>& values,
        std::index_sequence<Is...>) {
        return std::make_tuple(
            ArgConverter<TArgs>::from_string(values.at(m_arg_names[Is]))...);
    }

    TObj* m_obj;
    MethodPtr m_method;
    std::vector<std::string> m_arg_names;
    std::map<std::string, std::string> m_defaults;
};

template <typename TObj, typename TRet, typename... TArgs>
class ConstWrapper : public WrapperBase {
public:
    using MethodPtr = TRet(TObj::*)(TArgs...) const;

    ConstWrapper(const TObj* obj, MethodPtr method,
        std::vector<std::string> arg_names,
        std::map<std::string, std::string> defaults)
        : m_obj(obj),
        m_method(method),
        m_arg_names(std::move(arg_names)),
        m_defaults(std::move(defaults)) {
        if (m_arg_names.size() != sizeof...(TArgs)) {
            throw std::invalid_argument("Arg names count mismatch");
        }
    }

    [[nodiscard]] std::string execute(
        const std::map<std::string, std::string>& overrides) override {
        std::map<std::string, std::string> merged = m_defaults;
        for (const auto& kv : overrides) {
            merged[kv.first] = kv.second;
        }
        auto args = build_args(merged, std::index_sequence_for<TArgs...>{});
        TRet result = std::apply(
            m_method, std::tuple_cat(std::make_tuple(m_obj), args));
        return ArgConverter<TRet>::to_string(result);
    }

private:
    template <std::size_t... Is>
    auto build_args(const std::map<std::string, std::string>& values,
        std::index_sequence<Is...>) {
        return std::make_tuple(
            ArgConverter<TArgs>::from_string(values.at(m_arg_names[Is]))...);
    }

    const TObj* m_obj;
    MethodPtr m_method;
    std::vector<std::string> m_arg_names;
    std::map<std::string, std::string> m_defaults;
};