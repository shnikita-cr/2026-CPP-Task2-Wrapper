#ifndef WRAPPER_H
#define WRAPPER_H

#include <functional>
#include <iterator>
#include <map>
#include <stdexcept>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

using Arguments = std::map<std::string, std::string>;

class Wrapper {
public:
    template <typename Object, typename Result, typename... Parameters>
    Wrapper(Object* object,
            Result (Object::*method)(Parameters...) const,
            Arguments defaults)
        : defaults_(std::move(defaults)) {
        validateMethod(object, method);
        validateDefaultCount<Parameters...>();
        invoker_ = [object, method](const Arguments& arguments) {
            return invoke(object, method, arguments);
        };
    }

    template <typename Object, typename Result, typename... Parameters>
    Wrapper(Object* object,
            Result (Object::*method)(Parameters...),
            Arguments defaults)
        : defaults_(std::move(defaults)) {
        validateMethod(object, method);
        validateDefaultCount<Parameters...>();
        invoker_ = [object, method](const Arguments& arguments) {
            return invoke(object, method, arguments);
        };
    }

    std::string execute(const Arguments& overrides = {}) const {
        Arguments arguments = defaults_;
        for (const auto& [name, value] : overrides) {
            const auto defaultValue = defaults_.find(name);
            if (defaultValue == defaults_.end()) {
                throw std::invalid_argument("Unknown argument: " + name);
            }
            arguments[name] = value;
        }
        return invoker_(arguments);
    }

private:
    template <typename Object, typename Method>
    static void validateMethod(Object* object, Method method) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Wrapper object and method must be valid");
        }
    }

    template <typename... Parameters>
    void validateDefaultCount() const {
        if constexpr (!isArgumentMap<Parameters...>()) {
            if (defaults_.size() != sizeof...(Parameters)) {
                throw std::invalid_argument("Default argument count does not match method");
            }
        }
    }

    template <typename... Parameters>
    static constexpr bool isArgumentMap() {
        return sizeof...(Parameters) == 1 &&
               (std::is_same_v<std::decay_t<Parameters>, Arguments> && ...);
    }

    template <typename Object, typename Result, typename... Parameters>
    static std::string invoke(Object* object,
                              Result (Object::*method)(Parameters...) const,
                              const Arguments& arguments) {
        if constexpr (isArgumentMap<Parameters...>()) {
            return invokeCall([&]() { return std::invoke(method, object, arguments); });
        } else {
            return invokeStringArguments(object, method, arguments,
                                        std::index_sequence_for<Parameters...>{});
        }
    }

    template <typename Object, typename Result, typename... Parameters>
    static std::string invoke(Object* object,
                              Result (Object::*method)(Parameters...),
                              const Arguments& arguments) {
        if constexpr (isArgumentMap<Parameters...>()) {
            return invokeCall([&]() { return std::invoke(method, object, arguments); });
        } else {
            return invokeStringArguments(object, method, arguments,
                                        std::index_sequence_for<Parameters...>{});
        }
    }

    template <typename Object, typename Result, typename... Parameters,
              std::size_t... Indices>
    static std::string invokeStringArguments(
        Object* object, Result (Object::*method)(Parameters...) const,
        const Arguments& arguments, std::index_sequence<Indices...>) {
        if (arguments.size() != sizeof...(Parameters)) {
            throw std::invalid_argument("Argument count does not match method");
        }
        // Positional method parameters follow the map's key order.
        const auto first = arguments.begin();
        return invokeCall([&]() -> decltype(auto) {
            return std::invoke(method, object,
                               parseArgument<Parameters>(std::next(first, Indices)->second)...);
        });
    }

    template <typename Object, typename Result, typename... Parameters,
              std::size_t... Indices>
    static std::string invokeStringArguments(
        Object* object, Result (Object::*method)(Parameters...),
        const Arguments& arguments, std::index_sequence<Indices...>) {
        if (arguments.size() != sizeof...(Parameters)) {
            throw std::invalid_argument("Argument count does not match method");
        }
        const auto first = arguments.begin();
        return invokeCall([&]() -> decltype(auto) {
            return std::invoke(method, object,
                               parseArgument<Parameters>(std::next(first, Indices)->second)...);
        });
    }

    template <typename Parameter>
    static Parameter parseArgument(const std::string& value) {
        using Value = std::decay_t<Parameter>;
        if constexpr (std::is_same_v<Value, std::string>) {
            return value;
        } else {
            static_assert(std::is_arithmetic_v<Value>,
                          "Wrapper parameters must be std::string or arithmetic types");
            std::istringstream input(value);
            Value parsed{};
            if (!(input >> parsed)) {
                throw std::invalid_argument("Cannot convert argument value: " + value);
            }
            input >> std::ws;
            if (!input.eof()) {
                throw std::invalid_argument("Unexpected characters in argument value: " + value);
            }
            return parsed;
        }
    }

    template <typename Callable>
    static std::string invokeCall(Callable&& callable) {
        using Result = std::invoke_result_t<Callable>;
        if constexpr (std::is_void_v<Result>) {
            std::forward<Callable>(callable)();
            return {};
        } else {
            const auto result = std::forward<Callable>(callable)();
            return stringify(result);
        }
    }

    template <typename Value>
    static std::string stringify(const Value& value) {
        if constexpr (std::is_same_v<std::decay_t<Value>, std::string>) {
            return value;
        } else if constexpr (std::is_same_v<std::decay_t<Value>, bool>) {
            return value ? "true" : "false";
        } else {
            std::ostringstream output;
            output << value;
            return output.str();
        }
    }

    Arguments defaults_;
    std::function<std::string(const Arguments&)> invoker_;
};

#endif  // WRAPPER_H
