#ifndef WRAPPER_H
#define WRAPPER_H

#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>
#include "argument_set.h"
#include "value.h"

/// Результат метода преобразуется в строку (void -> пустая строка).
class Wrapper {
public:
    template <typename Object, typename Class, typename Result, typename... Params>
    Wrapper(Object* object, Result (Class::*method)(Params...), ArgumentSet defaults)
        : Wrapper(make_invoker<Result, Params...>(object, method, std::index_sequence_for<Params...>{}),
                  std::move(defaults), sizeof...(Params)) {}

    template <typename Object, typename Class, typename Result, typename... Params>
    Wrapper(Object* object, Result (Class::*method)(Params...) const, ArgumentSet defaults)
        : Wrapper(make_invoker<Result, Params...>(object, method, std::index_sequence_for<Params...>{}),
                  std::move(defaults), sizeof...(Params)) {}

    [[nodiscard]] std::string call(const ArgumentSet& overrides = {}) const;

private:
    using Invoker = std::function<std::string(const ArgumentSet&)>;

    Wrapper(Invoker invoker, ArgumentSet defaults, std::size_t parameter_count);

    template <typename Result, typename... Params, typename Object, typename Method, std::size_t... Index>
    static Invoker make_invoker(Object* object, Method method, std::index_sequence<Index...> /*indices*/) {
        static_assert(((!std::is_lvalue_reference_v<Params> || std::is_const_v<std::remove_reference_t<Params>>) && ...),
                      "non-const lvalue reference parameters are not supported");
        return [object, method]([[maybe_unused]] const ArgumentSet& arguments) -> std::string {
            if constexpr (std::is_void_v<Result>) {
                std::invoke(method, object, arguments.value_at(Index).template as<std::decay_t<Params>>()...);
                return {};
            } else {
                return detail::to_text(
                    std::invoke(method, object, arguments.value_at(Index).template as<std::decay_t<Params>>()...));
            }
        };
    }

    ArgumentSet m_defaults;
    Invoker m_invoker;
};

#endif