#pragma once

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <utility>

#include "Convert.h"
#include "ArgsMap.h"

template<typename T, typename R, typename... Args>
class Wrapper {
private:
    std::function<R(Args...)> function;

    ArgsMap defaultArgs;

    ArgsMap merge(const ArgsMap& newArgs) const {
        ArgsMap mergedArgs = defaultArgs;

        for (const auto& argument : newArgs) {
            const auto& key = argument.first;

            auto it = std::find_if(mergedArgs.begin(), mergedArgs.end(),
                [&key](const auto& arg) {
                    return arg.first == key;
                }
            );

            if (it == mergedArgs.end()) {
                throw std::invalid_argument("Unknown argument: " + key);
            }

            it->second = argument.second;
        }

        return mergedArgs;
    }

    template<std::size_t... I>
    R callMethod(const ArgsMap& args, std::index_sequence<I...>) {
        return function( convert<Args>(args[I].second)...);
    }

    void validateDefaults() const {
        if (defaultArgs.size() != sizeof...(Args)) {
            throw std::invalid_argument("Wrong number of default arguments");
        }

        for (std::size_t i = 0; i < defaultArgs.size(); ++i) {
            for (std::size_t j = i + 1; j < defaultArgs.size(); ++j) {
                if (defaultArgs[i].first == defaultArgs[j].first) {
                    throw std::invalid_argument(
                        "Duplicate argument name"
                    );
                }
            }
        }
    }

public:

    Wrapper(T* object, R(T::* method)(Args...), ArgsMap defaults): 
        function([object, method](Args... args) -> R {
            return (object->*method)(std::forward<Args>(args)...);
        }),
        defaultArgs(std::move(defaults)) 
    {

        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Null object or method");
        }

        validateDefaults();
    }

    Wrapper(const T* object, R(T::* method)(Args...) const,ArgsMap defaults): 
        function([object, method](Args... args) -> R {
            return (object->*method)(std::forward<Args>(args)...);
        }),
        defaultArgs(std::move(defaults)) 
    {

        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Null object or method");
        }

        validateDefaults();
    }

    R invoke(const ArgsMap& newArgs) {
        auto mergedArgs = merge(newArgs);

        return callMethod(mergedArgs,std::make_index_sequence<sizeof...(Args)>{});
    }
};

template<typename T, typename R, typename... Args>
Wrapper(const T*, R(T::*)(Args...) const, ArgsMap) -> Wrapper<T, R, Args...>;