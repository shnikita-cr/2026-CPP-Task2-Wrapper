#pragma once

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

// Аргументы передаются по именам, как и требуется в задании.
using Arguments = std::map<std::string, std::string>;

class Wrapper {
public:
    template <typename T>
    Wrapper(T* object, std::string (T::*method)(const Arguments&),
            Arguments defaults = {})
        : defaults_(std::move(defaults)) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Wrapper: null object or method");
        }
        call_ = [object, method](const Arguments& args) {
            return (object->*method)(args);
        };
    }

    template <typename T>
    Wrapper(const T* object, std::string (T::*method)(const Arguments&) const,
            Arguments defaults = {})
        : defaults_(std::move(defaults)) {
        if (object == nullptr || method == nullptr) {
            throw std::invalid_argument("Wrapper: null object or method");
        }
        call_ = [object, method](const Arguments& args) {
            return (object->*method)(args);
        };
    }

    [[nodiscard]] std::string invoke(const Arguments& overrides = {}) const {
        Arguments actual = defaults_;
        for (const auto& item : overrides) {
            actual.insert_or_assign(item.first, item.second);
        }
        return call_(actual);
    }

private:
    Arguments defaults_;
    std::function<std::string(const Arguments&)> call_;
};
