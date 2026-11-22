#pragma once

#include <cstddef>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
std::string to_text(const T& value) {
    if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else {
        std::ostringstream output;
        output << value;
        return output.str();
    }
}

class ArgValue {
public:
    template <typename T>
    ArgValue(const T& value) : text_(to_text(value)) {}

    [[nodiscard]] const std::string& text() const {
        return text_;
    }

private:
    std::string text_;
};

using ArgumentList = std::vector<std::pair<std::string, ArgValue>>;
using TextArgument = std::pair<std::string, std::string>;
using TextList = std::vector<TextArgument>;

[[nodiscard]] inline std::invalid_argument bad_value(const TextArgument& argument) {
    return std::invalid_argument("Argument '" + argument.first + "' has invalid value '" +
                                 argument.second + "'");
}

template <typename T>
T convert(const TextArgument& argument) {
    std::istringstream input(argument.second);
    T result{};
    input >> result;
    const bool parsed = !input.fail();
    input >> std::ws;
    if (!parsed || !input.eof()) {
        throw bad_value(argument);
    }
    return result;
}

template <>
inline std::string convert<std::string>(const TextArgument& argument) {
    return argument.second;
}

template <>
inline bool convert<bool>(const TextArgument& argument) {
    if (argument.second == "true" || argument.second == "1") {
        return true;
    }
    if (argument.second == "false" || argument.second == "0") {
        return false;
    }
    throw bad_value(argument);
}

class Wrapper {
public:
    template <typename Object, typename Class, typename Result, typename... Args>
    Wrapper(Object* object, Result (Class::*method)(Args...), ArgumentList defaults)
        : defaults_(std::move(defaults)) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Object and method must not be null");
        }
        prepare(make_callable<Result, Args...>(object, method));
    }

    template <typename Object, typename Class, typename Result, typename... Args>
    Wrapper(Object* object, Result (Class::*method)(Args...) const, ArgumentList defaults)
        : defaults_(std::move(defaults)) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Object and method must not be null");
        }
        prepare(make_callable<Result, Args...>(object, method));
    }

    [[nodiscard]] std::string execute(const ArgumentList& overrides = {}) const;

private:
    template <typename Result, typename... Args, typename Object, typename Method>
    static std::function<Result(Args...)> make_callable(Object* object, Method method) {
        return [object, method](Args... args) -> Result {
            if constexpr (std::is_void_v<Result>) {
                (object->*method)(std::forward<Args>(args)...);
            } else {
                return (object->*method)(std::forward<Args>(args)...);
            }
        };
    }

    template <typename Result, typename... Args>
    void prepare(std::function<Result(Args...)> callable) {
        check_defaults(sizeof...(Args));
        check_types<Args...>(texts_from_defaults(), std::index_sequence_for<Args...>{});
        invoker_ = [callable = std::move(callable)](const TextList& arguments) {
            return call_function(callable, arguments, std::index_sequence_for<Args...>{});
        };
    }

    template <typename... Args, std::size_t... Index>
    static void check_types([[maybe_unused]] const TextList& arguments,
                            [[maybe_unused]] std::index_sequence<Index...> indices) {
        (static_cast<void>(convert<std::decay_t<Args>>(arguments[Index])), ...);
    }

    template <typename Result, typename... Args, std::size_t... Index>
    static std::string call_function(const std::function<Result(Args...)>& callable,
                                     [[maybe_unused]] const TextList& arguments,
                                     [[maybe_unused]] std::index_sequence<Index...> indices) {
        if constexpr (std::is_void_v<Result>) {
            callable(convert<std::decay_t<Args>>(arguments[Index])...);
            return {};
        } else {
            return to_text(callable(convert<std::decay_t<Args>>(arguments[Index])...));
        }
    }

    void check_defaults(std::size_t arity) const;
    [[nodiscard]] std::size_t position_of(const std::string& name) const;
    [[nodiscard]] TextList texts_from_defaults() const;

    ArgumentList defaults_;
    std::function<std::string(const TextList&)> invoker_;
};