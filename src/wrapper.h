#ifndef WRAPPER_H
#define WRAPPER_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "value.h"

// Хранит объект произвольного класса, указатель на его метод
// и список аргументов со значениями по умолчанию.
class Wrapper {
public:
    template <typename Class, typename Ret, typename... Args>
    Wrapper(Class* object, Ret (Class::*method)(Args...), ArgList defaults) {
        bind(std::function<Ret(Args...)>([object, method](Args... args) -> Ret {
                 return (object->*method)(std::forward<Args>(args)...);
             }),
             std::move(defaults));
    }

    template <typename Class, typename Ret, typename... Args>
    Wrapper(Class* object, Ret (Class::*method)(Args...) const, ArgList defaults) {
        bind(std::function<Ret(Args...)>([object, method](Args... args) -> Ret {
                 return (object->*method)(std::forward<Args>(args)...);
             }),
             std::move(defaults));
    }

    // Вызов метода: переданные аргументы заменяют значения по умолчанию.
    [[nodiscard]] Value call(const ArgList& overrides) const;

private:
    using Invoker = std::function<Value(const std::vector<Value>&)>;

    template <typename Ret, typename... Args>
    void bind(std::function<Ret(Args...)> func, ArgList defaults) {
        if (defaults.size() != sizeof...(Args)) {
            throw std::invalid_argument("Defaults count must match method parameters count");
        }
        defaults_ = std::move(defaults);
        invoker_ = [func = std::move(func)](const std::vector<Value>& values) -> Value {
            return invoke(func, values, std::index_sequence_for<Args...>{});
        };
    }

    template <typename Ret, typename... Args, std::size_t... Idx>
    static Value invoke(const std::function<Ret(Args...)>& func,
                        [[maybe_unused]] const std::vector<Value>& values,
                        std::index_sequence<Idx...> /*indices*/) {
        if constexpr (std::is_void_v<Ret>) {
            func(values[Idx].template as<std::decay_t<Args>>()...);
            return Value("");
        } else {
            return Value(func(values[Idx].template as<std::decay_t<Args>>()...));
        }
    }

    ArgList defaults_;
    Invoker invoker_;
};

#endif // WRAPPER_H
