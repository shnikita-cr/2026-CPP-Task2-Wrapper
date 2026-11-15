#pragma once
#include "argument.h"
#include <functional>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <initializer_list>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using Arguments = std::map<std::string, Argument>;

class Wrapper {
public:
    template <typename Object, typename Method>
    Wrapper(Object* object, Method method,
            std::initializer_list<std::pair<std::string, Argument>> defaults) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Null object or method");
        }
        for (const auto& entry : defaults) {
            if (entry.first.empty() || !defaults_.emplace(entry).second) {
                throw std::invalid_argument("Empty or duplicate argument name");
            }
            names_.push_back(entry.first);
        }
        bind(object, method);
    }
    [[nodiscard]] std::string execute(const Arguments& overrides = {}) const {
        auto values = defaults_;
        for (const auto& entry : overrides) {
            if (values.count(entry.first) == 0) {
                throw std::invalid_argument("Unknown argument: " + entry.first);
            }
            values.at(entry.first) = entry.second;
        }
        return invoke_(values);
    }
private:
    template <typename... Args, typename Object, typename Method, std::size_t... Index>
    void bind_indices(Object* object, Method method, std::index_sequence<Index...> ) {
        if (names_.size() != sizeof...(Args)) {
            throw std::invalid_argument("Argument count does not match method");
        }
        invoke_ = [object, method, names = names_](const Arguments& values) {
            auto parameters = std::make_tuple(values.at(names[Index]).get<Args>()...);
            auto call = [object, method](auto&... arguments) {
                return std::invoke(method, object, arguments...);
            };
            using Result = std::invoke_result_t<Method, Object*, std::decay_t<Args>&...>;
            if constexpr (std::is_void_v<Result>) {
                std::apply(call, parameters);
                return std::string{};
            } else {
                std::ostringstream output;
                output << std::boolalpha << std::apply(call, parameters);
                return output.str();
            }
        };
    }
    template <typename Object, typename Class, typename Result, typename... Args>
    void bind(Object* object, Result (Class::*method)(Args...)) {
        bind_indices<Args...>(object, method, std::index_sequence_for<Args...>{});
    }
    template <typename Object, typename Class, typename Result, typename... Args>
    void bind(Object* object, Result (Class::*method)(Args...) const) {
        bind_indices<Args...>(object, method, std::index_sequence_for<Args...>{});
    }
    Arguments defaults_;
    std::vector<std::string> names_;
    std::function<std::string(const Arguments&)> invoke_;
};
