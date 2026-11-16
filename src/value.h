#ifndef VALUE_H
#define VALUE_H

#include <any>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// ошибка преобразования значения аргумента к типу параметра метода
class ConversionError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

namespace detail {

template <typename T, typename = void>
struct IsStreamable : std::false_type {};

template <typename T>
struct IsStreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>
    : std::true_type {};

template <typename T>
inline constexpr bool kAlwaysFalse = false;

// текстовое представление значения
template <typename T>
std::string to_text(const T& value) {
    if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else if constexpr (std::is_convertible_v<const T&, std::string>) {
        return std::string(value);
    } else if constexpr (IsStreamable<T>::value) {
        std::ostringstream output;
        output << value;
        return output.str();
    } else {
        static_assert(kAlwaysFalse<T>, "the type has neither std::string conversion nor operator<<");
        return {};
    }
}

template <typename Callback>
bool visit_arithmetic(const std::any& data, Callback&& callback) {
    bool matched = false;
    const auto probe = [&data, &callback, &matched](auto sample) {
        using Type = decltype(sample);
        if (const Type* held = std::any_cast<Type>(&data); held != nullptr) {
            callback(*held);
            matched = true;
        }
    };
    const auto probe_all = [&probe](auto... samples) { (probe(samples), ...); };
    probe_all(false, '\0', short{}, 0, 0L, 0LL, static_cast<unsigned short>(0), 0U, 0UL, 0ULL, 0.0F, 0.0, 0.0L);
    return matched;
}

template <typename T>
std::optional<T> parse_text(const std::string& text) {
    if constexpr (std::is_same_v<T, bool>) {
        if (text == "true" || text == "1") {
            return true;
        }
        if (text == "false" || text == "0") {
            return false;
        }
        return std::nullopt;
    } else {
        std::istringstream input(text);
        T parsed{};
        if (input >> parsed && (input >> std::ws).eof()) {
            return parsed;
        }
        return std::nullopt;
    }
}

}

// значение аргумента произвольного типа
class Value {
public:
    Value() = default;

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Value>>>
    Value(T&& value)
        : m_data(store(std::forward<T>(value))) {}

    // извлечение значения типа T
    template <typename T>
    [[nodiscard]] std::optional<T> try_as() const {
        if (const T* direct = std::any_cast<T>(&m_data); direct != nullptr) {
            return *direct;
        }
        return convert<T>();
    }

    template <typename T>
    [[nodiscard]] T as() const {
        if (std::optional<T> result = try_as<T>(); result.has_value()) {
            return *std::move(result);
        }
        throw ConversionError("cannot convert value " + describe() + " to the requested type");
    }
    [[nodiscard]] std::string describe() const;

private:
    template <typename T>
    static std::any store(T&& value) {
        using Plain = std::decay_t<T>;
        if constexpr (std::is_convertible_v<Plain, std::string_view> && !std::is_same_v<Plain, std::string>) {
            return std::any(std::string(std::string_view(value)));
        } else {
            return std::any(std::forward<T>(value));
        }
    }

    template <typename T>
    [[nodiscard]] std::optional<T> convert() const {
        std::optional<T> result;
        if constexpr (std::is_same_v<T, std::string>) {
            detail::visit_arithmetic(m_data, [&result](const auto& number) { result = detail::to_text(number); });
        } else if constexpr (std::is_arithmetic_v<T>) {
            if (const auto* text = std::any_cast<std::string>(&m_data); text != nullptr) {
                result = detail::parse_text<T>(*text);
            } else {
                detail::visit_arithmetic(m_data, [&result](const auto& number) { result = static_cast<T>(number); });
            }
        }
        return result;
    }

    std::any m_data;
};

#endif