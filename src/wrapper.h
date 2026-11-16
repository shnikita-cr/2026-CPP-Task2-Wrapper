#ifndef WRAPPER_H
#define WRAPPER_H

#include <cstddef>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


using ArgumentList = std::vector<std::pair<std::string, std::string>>;

using ArgumentMap = std::map<std::string, std::string>;

namespace detail {

template <typename Result>
std::string to_result(Result&& value) {
    using Decayed = std::decay_t<Result>;
    if constexpr (std::is_same_v<Decayed, bool>) {
        return value ? "true" : "false";
    } else if constexpr (std::is_arithmetic_v<Decayed>) {
        return std::to_string(value);
    } else {
        static_assert(std::is_constructible_v<std::string, Result>,
                      "Wrapper: method result must be convertible to std::string");
        return std::string(std::forward<Result>(value));
    }
}

} 

class Wrapper {
public:
    template <typename Subject, typename Result, typename... Args>
    Wrapper(Subject* subject, Result (Subject::*method)(Args...), ArgumentList defaults)
        : Wrapper(make_invoker(subject, method, std::index_sequence_for<Args...>{}),
                  std::move(defaults), sizeof...(Args)) {
        static_assert((std::is_convertible_v<const std::string&, Args> && ...),
                      "Wrapper: all method parameters must accept std::string");
    }

    template <typename Subject, typename Result, typename... Args>
    Wrapper(Subject* subject, Result (Subject::*method)(Args...) const, ArgumentList defaults)
        : Wrapper(make_invoker(subject, method, std::index_sequence_for<Args...>{}),
                  std::move(defaults), sizeof...(Args)) {
        static_assert((std::is_convertible_v<const std::string&, Args> && ...),
                      "Wrapper: all method parameters must accept std::string");
    }


    [[nodiscard]] std::string execute(const ArgumentMap& overrides = {}) const;

    [[nodiscard]] std::vector<std::string> argument_names() const;
    [[nodiscard]] const ArgumentList& defaults() const;

private:
    using Invoker = std::function<std::string(const std::vector<std::string>&)>;

    Wrapper(Invoker invoker, ArgumentList defaults, std::size_t arity);

    template <typename Subject, typename Method, std::size_t... Index>
    static Invoker make_invoker(Subject* subject, Method method,
                                std::index_sequence<Index...> /*indices*/) {
        if (subject == nullptr) {
            throw std::invalid_argument("Wrapper: subject pointer is nullptr");
        }
        if (method == nullptr) {
            throw std::invalid_argument("Wrapper: method pointer is nullptr");
        }
        return [subject, method]([[maybe_unused]] const std::vector<std::string>& values) {
            if constexpr (std::is_void_v<decltype((subject->*method)(values[Index]...))>) {
                (subject->*method)(values[Index]...);
                return std::string{};
            } else {
                return detail::to_result((subject->*method)(values[Index]...));
            }
        };
    }

    Invoker invoker_;
    ArgumentList defaults_;
};

#endif  
