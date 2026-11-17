#ifndef ARG_VALUE_H
#define ARG_VALUE_H

#include <any>
#include <initializer_list>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

class ArgValue;

template <typename Target>
struct ValueCast {
    static Target cast(const ArgValue& val);
};

class ArgValue {
private:
    std::any value_;
    std::string str_repr_;

public:
    ArgValue();
    // cppcheck-suppress noExplicitConstructor
    ArgValue(int val);                     // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgValue(long val);                    // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgValue(double val);                  // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgValue(float val);                   // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgValue(bool val);                    // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgValue(const char* val);             // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgValue(const std::string& val);      // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgValue(std::string&& val) noexcept;  // NOLINT(google-explicit-constructor)

    template <typename T, typename = std::enable_if_t<
        !std::is_same_v<std::decay_t<T>, ArgValue> &&
        !std::is_same_v<std::decay_t<T>, const char*> &&
        !std::is_same_v<std::decay_t<T>, std::string> &&
        !std::is_arithmetic_v<std::decay_t<T>>>>
    explicit ArgValue(const T& val) : value_(val) {}

    template <typename T>
    T as() const {
        return ValueCast<std::decay_t<T>>::cast(*this);
    }

    const std::string& to_string() const noexcept;
    bool has_value() const noexcept;
    const std::any& raw_any() const noexcept;

    int as_int() const;
    long as_long() const;
    double as_double() const;
    float as_float() const;
    bool as_bool() const;
};

template <typename Target>
Target ValueCast<Target>::cast(const ArgValue& val) {
    return std::any_cast<Target>(val.raw_any());
}

template <>
struct ValueCast<std::string> {
    static std::string cast(const ArgValue& val) {
        return val.to_string();
    }
};

template <>
struct ValueCast<const char*> {
    static const char* cast(const ArgValue& val) {
        return val.to_string().c_str();
    }
};

template <>
struct ValueCast<int> {
    static int cast(const ArgValue& val) {
        return val.as_int();
    }
};

template <>
struct ValueCast<long> {
    static long cast(const ArgValue& val) {
        return val.as_long();
    }
};

template <>
struct ValueCast<double> {
    static double cast(const ArgValue& val) {
        return val.as_double();
    }
};

template <>
struct ValueCast<float> {
    static float cast(const ArgValue& val) {
        return val.as_float();
    }
};

template <>
struct ValueCast<bool> {
    static bool cast(const ArgValue& val) {
        return val.as_bool();
    }
};

class ArgsMap {
private:
    std::vector<std::pair<std::string, ArgValue>> entries_;

public:
    ArgsMap() = default;
    // cppcheck-suppress noExplicitConstructor
    ArgsMap(std::initializer_list<std::pair<std::string, ArgValue>> init); // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgsMap(const std::vector<std::pair<std::string, ArgValue>>& entries); // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgsMap(const std::map<std::string, ArgValue>& map);                  // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgsMap(const std::map<std::string, std::string>& map);               // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgsMap(const std::unordered_map<std::string, ArgValue>& map);        // NOLINT(google-explicit-constructor)
    // cppcheck-suppress noExplicitConstructor
    ArgsMap(const std::unordered_map<std::string, std::string>& map);     // NOLINT(google-explicit-constructor)

    const ArgValue* find(const std::string& name) const noexcept;
    bool has(const std::string& name) const noexcept;
    const ArgValue& at(const std::string& name) const;
    std::size_t size() const noexcept;
    bool empty() const noexcept;
    const std::vector<std::pair<std::string, ArgValue>>& entries() const noexcept;
};

#endif // ARG_VALUE_H
