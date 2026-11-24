#pragma once

#include <any>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

class Wrapper {
public:
    using Args = std::map<std::string, std::any, std::less<>>;
    using Callback = std::function<std::string(const Args&)>;

    template <typename Object, typename Result>
    Wrapper(Object* object, Result(Object::* method)(const Args&), Args defaults = {})
        : defaults_(std::move(defaults)),
        callback_([object, method](const Args& args) -> std::string {
        if (object == nullptr) {
            throw std::runtime_error("null");
        }
        return (object->*method)(args);
            }) {}

    template <typename Object, typename Result>
    Wrapper(const Object* object, Result(Object::* method)(const Args&) const, Args defaults = {})
        : defaults_(std::move(defaults)),
        callback_([object, method](const Args& args) -> std::string {
        if (object == nullptr) {
            throw std::runtime_error("null");
        }
        return (object->*method)(args);
            }) {}

    [[nodiscard]] std::string invoke(const Args& overrides = {}) const {
        Args merged = defaults_;
        for (const auto& [key, value] : overrides) {
            merged[key] = value;
        }
        return callback_(merged);
    }

private:
    Args defaults_;
    Callback callback_;
};