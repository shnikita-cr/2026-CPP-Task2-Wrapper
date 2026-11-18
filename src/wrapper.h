#ifndef WRAPPER_H
#define WRAPPER_H

#include <any>
#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <unordered_set>
#include <vector>

using ArgumentMap = std::map<std::string, std::any>;

class Wrapper {
public:
    template <typename Object, typename Return, typename... Args>
    Wrapper(Object* object, Return (Object::*method)(Args...), ArgumentMap defaults,
            std::vector<std::string> argument_order = {})
        : invoke_() {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Wrapper requires a valid object and method");
        }
        normalize_c_string_values(defaults);
        auto names = normalize_order(argument_order, defaults);
        validate_argument_count<Args...>(names.size());
        validate_argument_names(names, defaults);
        invoke_ = [object, method, defaults = std::move(defaults),
                   argument_order = std::move(names)](const ArgumentMap& overrides) {
            return invoke_member(object, method, defaults, argument_order, overrides,
                                 static_cast<std::tuple<Args...>*>(nullptr),
                                 std::index_sequence_for<Args...>{});
        };
    }

    template <typename Object, typename Return, typename... Args>
    Wrapper(Object* object, Return (Object::*method)(Args...) const, ArgumentMap defaults,
            std::vector<std::string> argument_order = {})
        : invoke_() {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Wrapper requires a valid object and method");
        }
        normalize_c_string_values(defaults);
        auto names = normalize_order(argument_order, defaults);
        validate_argument_count<Args...>(names.size());
        validate_argument_names(names, defaults);
        invoke_ = [object, method, defaults = std::move(defaults),
                   argument_order = std::move(names)](const ArgumentMap& overrides) {
            return invoke_member(object, method, defaults, argument_order, overrides,
                                 static_cast<std::tuple<Args...>*>(nullptr),
                                 std::index_sequence_for<Args...>{});
        };
    }

    [[nodiscard]] std::string execute(const ArgumentMap& overrides = {}) const {
        return invoke_(overrides);
    }

private:
    std::function<std::string(const ArgumentMap&)> invoke_;

    template <typename T, typename = void>
    struct IsStreamExtractable : std::false_type {};

    template <typename T>
    struct IsStreamExtractable<
        T, std::void_t<decltype(std::declval<std::istringstream&>() >> std::declval<T&>())>>
        : std::true_type {};

    static std::vector<std::string> normalize_order(const std::vector<std::string>& order,
                                                     const ArgumentMap& defaults) {
        if (!order.empty()) {
            return order;
        }
        std::vector<std::string> result;
        result.reserve(defaults.size());
        for (const auto& item : defaults) {
            result.push_back(item.first);
        }
        return result;
    }

    static void normalize_c_string_values(ArgumentMap& arguments) {
        for (auto& item : arguments) {
            const std::type_info& value_type = item.second.type();
            if (value_type == typeid(const char*) || value_type == typeid(char*)) {
                item.second = any_to_string(item.second);
            }
        }
    }

    template <typename... Args>
    static void validate_argument_count(std::size_t count) {
        if (count != sizeof...(Args)) {
            throw std::invalid_argument("Argument names must match the method parameter count");
        }
    }

    static void validate_argument_names(const std::vector<std::string>& names,
                                        const ArgumentMap& defaults) {
        std::unordered_set<std::string> unique_names;
        for (const auto& name : names) {
            if (defaults.find(name) == defaults.end()) {
                throw std::invalid_argument("Missing default value for argument: " + name);
            }
            if (!unique_names.insert(name).second) {
                throw std::invalid_argument("Duplicate argument name: " + name);
            }
        }
        if (unique_names.size() != defaults.size()) {
            throw std::invalid_argument("Every default value must name a method parameter");
        }
    }

    template <typename T>
    static std::decay_t<T> parse_value(const std::any& value) {
        using Value = std::decay_t<T>;
        if (const auto* exact_value = std::any_cast<Value>(&value)) {
            return *exact_value;
        }
        if constexpr (std::is_same_v<Value, std::string>) {
            return any_to_string(value);
        } else if constexpr (std::is_same_v<Value, bool>) {
            const std::string text = any_to_string(value);
            if (text == "true" || text == "1") {
                return true;
            }
            if (text == "false" || text == "0") {
                return false;
            }
            throw std::invalid_argument("Boolean argument must be true, false, 1, or 0");
        } else if constexpr (IsStreamExtractable<Value>::value) {
            std::istringstream stream(any_to_string(value));
            Value result{};
            stream >> result;
            if (!stream || stream.peek() != std::char_traits<char>::eof()) {
                throw std::invalid_argument("Cannot convert argument value to requested type");
            }
            return result;
        } else {
            throw std::invalid_argument("Argument requires an exact std::any type");
        }
    }

    static std::string any_to_string(const std::any& value) {
        if (const auto* text = std::any_cast<std::string>(&value)) {
            return *text;
        }
        if (const auto* text = std::any_cast<const char*>(&value)) {
            return *text == nullptr ? std::string{} : std::string(*text);
        }
        if (const auto* text = std::any_cast<char*>(&value)) {
            return *text == nullptr ? std::string{} : std::string(*text);
        }
        throw std::invalid_argument("Argument value cannot be converted to text");
    }

    template <typename Return, typename Result>
    static std::string stringify_result(Result&& result) {
        if constexpr (std::is_same_v<std::decay_t<Return>, std::string>) {
            return std::forward<Result>(result);
        } else if constexpr (std::is_same_v<std::decay_t<Return>, const char*>) {
            return result == nullptr ? std::string{} : std::string(result);
        } else if constexpr (std::is_same_v<std::decay_t<Return>, bool>) {
            return result ? "true" : "false";
        } else {
            std::ostringstream stream;
            stream << std::forward<Result>(result);
            return stream.str();
        }
    }

    template <typename Object, typename Method, typename... Args, std::size_t... Indices>
    static std::string invoke_member(Object* object, Method method,
                                     const ArgumentMap& defaults,
                                     const std::vector<std::string>& names,
                                     const ArgumentMap& overrides,
                                     std::tuple<Args...>*,
                                     std::index_sequence<Indices...>) {
        for (const auto& item : overrides) {
            if (defaults.find(item.first) == defaults.end()) {
                throw std::invalid_argument("Unknown argument: " + item.first);
            }
        }

        auto values = std::make_tuple(
            parse_value<Args>(value_for(names[Indices], defaults, overrides))...);
        return invoke_with_arguments<Args...>(object, method, values);
    }

    template <typename... Args, typename Object, typename Method, typename... Values>
    static std::string invoke_with_arguments(Object* object, Method method,
                                             std::tuple<Values...>& values) {
        using Return = std::invoke_result_t<Method, Object*, Args...>;
        if constexpr (std::is_void_v<Return>) {
            std::apply(
                [object, method](auto&... args) {
                    std::invoke(method, object, std::forward<Args>(args)...);
                },
                values);
            return {};
        } else {
            decltype(auto) result = std::apply(
                [object, method](auto&... args) -> decltype(auto) {
                    return std::invoke(method, object, std::forward<Args>(args)...);
                }, values);
            return stringify_result<Return>(std::forward<decltype(result)>(result));
        }
    }

    static const std::any& value_for(const std::string& name,
                                     const ArgumentMap& defaults,
                                     const ArgumentMap& overrides) {
        const auto override_value = overrides.find(name);
        return override_value == overrides.end() ? defaults.at(name) : override_value->second;
    }
};

#endif
