#ifndef VALUE_H
#define VALUE_H

#include <any>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace detail {

template <typename T, typename = void>
struct IsStreamable : std::false_type {};

template <typename T>
struct IsStreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>
    : std::true_type {};

template <typename T>
std::string printAny(const std::any& data) {
    if constexpr (IsStreamable<T>::value) {
        std::ostringstream stream;
        stream << std::any_cast<const T&>(data);
        return stream.str();
    } else {
        return typeid(T).name();
    }
}

}

class Value {
public:
    Value() = default;

    Value(const char* text) : Value(std::string(text)) {}

    template <typename T>
    Value(T value) : data_(std::move(value)), printer_(&detail::printAny<T>) {}

    template <typename T>
    [[nodiscard]] T get() const {
        const T* exact = std::any_cast<T>(&data_);
        if (exact != nullptr) {
            return *exact;
        }
        return convert<T>();
    }

    [[nodiscard]] std::string toString() const;

private:
    using Printer = std::string (*)(const std::any&);

    template <typename T>
    T convert() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return toString();
        } else if constexpr (std::is_arithmetic_v<T>) {
            return convertArithmetic<T>();
        } else {
            throwBadConversion(typeid(T).name());
        }
    }

    template <typename T>
    T convertArithmetic() const {
        T result{};
        if (!tryConvertFrom<T, bool, int, long, long long, unsigned, unsigned long, float, double>(result)) {
            throwBadConversion(typeid(T).name());
        }
        return result;
    }

    template <typename To, typename... From>
    bool tryConvertFrom(To& result) const {
        return (tryConvertOne<To, From>(result) || ...);
    }

    template <typename To, typename From>
    bool tryConvertOne(To& result) const {
        const From* source = std::any_cast<From>(&data_);
        if (source == nullptr) {
            return false;
        }
        result = static_cast<To>(*source);
        return true;
    }

    [[noreturn]] void throwBadConversion(const char* requested) const;

    std::any data_;
    Printer printer_ = nullptr;
};

#endif
