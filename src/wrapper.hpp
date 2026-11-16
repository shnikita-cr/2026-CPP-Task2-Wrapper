#pragma once

#include <functional>
#include <map>
#include <string>
#include <utility>

class Wrapper {
public:
    using ArgMap = std::map<std::string, std::string>;
    using Callable = std::function<std::string(const ArgMap&)>;

    Wrapper(Callable callable, ArgMap defaults);

    template <typename Object, typename Method>
    Wrapper(Object* object, Method method, ArgMap defaults)
        : callable_([object, method](const ArgMap& args) {
              return (object->*method)(args);
          }),
          defaults_(std::move(defaults)) {}

    [[nodiscard]] std::string invoke(const ArgMap& overrides) const;
    [[nodiscard]] const ArgMap& defaults() const noexcept;

private:
    Callable callable_;
    ArgMap defaults_;
};