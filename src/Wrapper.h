#pragma once

#include <any>
#include <algorithm>
#include <initializer_list>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>
#include <vector>

class Argument {
private:
    std::any m_value;

public:
    Argument() = default;
    Argument(const char* value) : m_value(std::string(value)) {}
    Argument(const std::string& value) : m_value(value) {}
    Argument(std::string&& value) : m_value(std::move(value)) {}

    template <class T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, Argument> && !std::is_convertible_v<T, const char*> && !std::is_same_v<std::decay_t<T>, std::string>, int> = 0>
    Argument(T&& value) : m_value(std::forward<T>(value)) {}

    template <class T>
    T get() const {
        try {
            return std::any_cast<T>(m_value);
        }
        catch (const std::bad_any_cast&) {
            throw std::invalid_argument("Argument type does not match method parameter");
        }
    }
};


class Arguments {
private:
    using value_type = std::pair<std::string, Argument>;
    using container_type = std::vector<value_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    container_type m_values;
public:
    Arguments() = default;

    Arguments(std::initializer_list<value_type> values) {
        for (const auto& value : values) {
            insert_or_assign(value.first, value.second);
        }
    }

    std::size_t size() const noexcept { return m_values.size(); }
    bool empty() const noexcept { return m_values.empty(); }

    iterator begin() noexcept { return m_values.begin(); }
    iterator end() noexcept { return m_values.end(); }
    const_iterator begin() const noexcept { return m_values.begin(); }
    const_iterator end() const noexcept { return m_values.end(); }
    const_iterator cbegin() const noexcept { return m_values.cbegin(); }
    const_iterator cend() const noexcept { return m_values.cend(); }

    iterator find(const std::string& key) {
        return std::find_if(m_values.begin(), m_values.end(), [&](const value_type& item) { return item.first == key; });
    }

    const_iterator find(const std::string& key) const {
        return std::find_if(m_values.begin(), m_values.end(), [&](const value_type& item) { return item.first == key; });
    }

    Argument& at(const std::string& key) {
        auto it = find(key);
        if (it == end()) throw std::out_of_range("Unknown argument name: " + key);
        return it->second;
    }

    const Argument& at(const std::string& key) const {
        auto it = find(key);
        if (it == end()) throw std::out_of_range("Unknown argument name: " + key);
        return it->second;
    }

    void insert_or_assign(const std::string& key, const Argument& value) {
        auto it = find(key);
        if (it == end()) m_values.emplace_back(key, value);
        else it->second = value;
    }
};

class WrapperBase {
public:
    virtual ~WrapperBase() = default;
    virtual std::string execute(const Arguments& new_arguments = {}) const = 0;
};

class Wrapper final : public WrapperBase {
private:
    template <class T>
    static std::string to_string_result(T&& value) {
        using Result = std::decay_t<T>;
        if constexpr (std::is_same_v<Result, std::string>) {
            return std::forward<T>(value);
        }
        else if constexpr (std::is_same_v<Result, const char*> ||
            std::is_same_v<Result, char*>) {
            return value ? std::string(value) : std::string{};
        }
        else if constexpr (std::is_arithmetic_v<Result>) {
            return std::to_string(value);
        }
        else {
            std::ostringstream stream;
            stream << std::forward<T>(value);
            return stream.str();
        }
    }

    template <class Class, class Return, class... Args>
    void initialize(Class* object,
        Return(Class::* method)(Args...),
        Arguments defaults) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Object and method must not be null");
        }
        initialize_impl(object, method, std::move(defaults), static_cast<std::tuple<Args...>*>(nullptr));
    }

    template <class Class, class Return, class... Args>
    void initialize(const Class* object,
        Return(Class::* method)(Args...) const,
        Arguments defaults) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Object and method must not be null");
        }
        initialize_impl(object, method, std::move(defaults), static_cast<std::tuple<Args...>*>(nullptr));
    }

    template <class Object, class Method, class... Args>
    void initialize_impl(Object* object, Method method, Arguments defaults, std::tuple<Args...>*) {
        if (defaults.size() != sizeof...(Args)) {
            throw std::invalid_argument("Number of default arguments must match method parameters");
        }

        std::vector<std::string> keys;
        keys.reserve(defaults.size());
        for (const auto& item : defaults) {
            keys.push_back(item.first);
        }

        m_executor = [object, method, defaults = std::move(defaults), keys = std::move(keys)](const Arguments& replacements) -> std::string {
            Arguments actual = defaults;
            for (const auto& replacement : replacements) {
                auto it = actual.find(replacement.first);
                if (it == actual.end()) {
                    throw std::invalid_argument("Unknown argument name: " + replacement.first);
                }
                it->second = replacement.second;
            }
            return invoke<Object, Method, Args...>(object, method, actual, keys, std::index_sequence_for<Args...>{});
            };
    }

    template <class Object, class Method, class... Args, std::size_t... I>
    static std::string invoke(Object* object, Method method, const Arguments& actual, const std::vector<std::string>& keys, std::index_sequence<I...>) {
        using Values = std::tuple<std::decay_t<Args>...>;
        Values values{ actual.at(keys[I]).template get<std::decay_t<Args>>()... };

        if constexpr (std::is_void_v<std::invoke_result_t<Method, Object*, Args...>>) {
            std::apply([&](auto&... args) {
                std::invoke(method, object, args...);
                }, values);
            return {};
        }
        else {
            auto result = std::apply([&](auto&... args) {
                return std::invoke(method, object, args...);
                }, values);
            return to_string_result(std::move(result));
        }
    }
    std::function<std::string(const Arguments&)> m_executor;

public:
    template <class Class, class Return, class... Args>
    Wrapper(Class* object, Return(Class::* method)(Args...), Arguments default_arguments) {
        initialize(object, method, std::move(default_arguments));
    }

    template <class Class, class Return, class... Args>
    Wrapper(const Class* object, Return(Class::* method)(Args...) const, Arguments default_arguments) {
        initialize(object, method, std::move(default_arguments));
    }

    std::string execute(const Arguments& new_arguments = {}) const override {
        if (!m_executor) {
            throw std::logic_error("Wrapper is not initialized");
        }
        return m_executor(new_arguments);
    }
};
