#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

template <typename T>
struct ArgConverter {
    static T from_string(const std::string& s) {
        std::istringstream iss(s);
        T value{};
        if (!(iss >> value)) {
            throw std::invalid_argument("Cannot parse argument: " + s);
        }
        return value;
    }
    static std::string to_string(const T& v) {
        std::ostringstream oss;
        oss << v;
        return oss.str();
    }
};

template <>
struct ArgConverter<std::string> {
    static std::string from_string(const std::string& s) { return s; }
    static std::string to_string(const std::string& v) { return v; }
};

template <>
struct ArgConverter<bool> {
    static bool from_string(const std::string& s) {
        if (s == "true" || s == "1") return true;
        if (s == "false" || s == "0") return false;
        throw std::invalid_argument("Cannot parse bool: " + s);
    }
    static std::string to_string(bool v) { return v ? "true" : "false"; }
};