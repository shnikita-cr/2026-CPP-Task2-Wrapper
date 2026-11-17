#ifndef WRAPPER_H
#define WRAPPER_H

#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include "argument_dict.h"

template <typename TClass, typename TReturn, typename... TArgs>
class Wrapper {
public:
    using MethodType = TReturn(TClass::*)(TArgs...);

    Wrapper(TClass* obj, MethodType method, std::vector<std::string> argNames, ArgumentDict defaults)
        : m_obj(obj), m_method(method), m_argNames(std::move(argNames)), m_defaults(std::move(defaults)) {
        if (m_argNames.size() != sizeof...(TArgs)) {
            throw std::invalid_argument("Wrapper: argNames count mismatch");
        }
    }

    std::string invoke(const ArgumentDict& overrides) const {
        ArgumentDict merged = mergeArgs(m_defaults, overrides);
        return callWithArgs(merged, std::index_sequence_for<TArgs...>{});
    }

private:
    template <std::size_t... Is>
    std::string callWithArgs(const ArgumentDict& args, std::index_sequence<Is...>) const {
        (void)args;
        return (m_obj->*m_method)(extract<TArgs>(args, Is)...);
    }

    template <typename T>
    std::decay_t<T> extract(const ArgumentDict& args, std::size_t index) const {
        const std::string& key = m_argNames.at(index);
        auto it = args.find(key);
        if (it == args.end()) {
            throw std::runtime_error("Missing argument: " + key);
        }
        return it->second.get<std::decay_t<T>>();
    }

    TClass* m_obj;
    MethodType m_method;
    std::vector<std::string> m_argNames;
    ArgumentDict m_defaults;
};

#endif