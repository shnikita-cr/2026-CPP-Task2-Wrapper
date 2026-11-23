#pragma once

#include <any>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

template<typename To>
To safeDoubleCast(double value) {static_assert( std::is_integral_v<To> && !std::is_same_v<To, bool>);

    if (!std::isfinite(value)) {
        throw std::invalid_argument("Cannot convert NaN or infinity to integer");
    }

    const long double x = static_cast<long double>(value);

    const long double lower = static_cast<long double>(std::numeric_limits<To>::lowest());

    const long double upperExclusive = std::ldexp(1.0L, std::numeric_limits<To>::digits);

    if (x < lower || x >= upperExclusive) {
        throw std::out_of_range("Double value is outside integer range");
    }

    return static_cast<To>(value);
}

template<typename Arg>
std::decay_t<Arg> convert(const std::any& value) {
    using Type = std::decay_t<Arg>;

    if (value.type() == typeid(Type)) {
        return std::any_cast<Type>(value);
    }

    if constexpr (std::is_arithmetic_v<Type>) {
        if (value.type() == typeid(int)) {
            return static_cast<Type>(std::any_cast<int>(value));
        }

        if (value.type() == typeid(double)) {
            if constexpr (std::is_integral_v<Type> && !std::is_same_v<Type, bool>) {
                return safeDoubleCast<Type>(std::any_cast<double>(value));
            }
            else {
                return static_cast<Type>(std::any_cast<double>(value));
            }
        }
    }

    if constexpr (std::is_same_v<Type, std::string>) {
        if (value.type() == typeid(const char*)) {
            return std::string(std::any_cast<const char*>(value));
        }
    }

    throw std::invalid_argument("Unsupported argument type");
}

template<typename T>
std::string resultToString(const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return value;
    }
    else if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    }
    else if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(value);
    }
    else {
        throw std::invalid_argument("Unsupported result type");
    }
}