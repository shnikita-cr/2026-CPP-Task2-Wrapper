#ifndef WRAPPER_WRAPPER_H
#define WRAPPER_WRAPPER_H

#include "arguments.h"
#include "command.h"
#include "method_traits.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <utility>

namespace detail
{

template <typename T> decltype(auto) bind_argument(const ArgumentValue& value)
{
    if constexpr (std::is_same_v<T, std::string_view>)
    {
        return std::string_view(value.as<std::string>());
    }
    else if constexpr (std::is_same_v<T, const char*>)
    {
        return value.as<std::string>().c_str();
    }
    else
    {
        return value.as<T>();
    }
}

} // namespace detail

template <typename Object, typename Method> class Wrapper final : public ICommand
{
    using Traits     = detail::MethodTraits<Method>;
    using Parameters = typename Traits::Parameters;
    using Indices    = std::make_index_sequence<Traits::arity>;

    static_assert(Traits::supported, "Wrapper requires a member method without ref qualifiers");
    static_assert(std::is_same_v<typename Traits::ReturnType, std::string>,
                  "Wrapper requires a method returning std::string");
    static_assert(Traits::parameters_supported,
                  "Wrapper parameters must be copyable values or const lvalue references; "
                  "mutable char* is not supported");
    static_assert(Traits::template accepts_object<Object, Method>,
                  "Wrapper object is incompatible with the method");

  public:
    Wrapper(Object* object, Method method, ArgumentList defaults)
        : object_(object), method_(method), defaults_(std::move(defaults))
    {
        if (object_ == nullptr || method_ == nullptr)
        {
            throw std::invalid_argument("Wrapper requires a non-null object and method");
        }
        if (defaults_.size() != Traits::arity)
        {
            throw std::invalid_argument("Default argument count does not match method arity");
        }

        validate_names();
        (void)bind_arguments(defaults_, Indices{});
    }

    [[nodiscard]] std::string execute(const ArgumentMap& overrides = {}) const override
    {
        ArgumentList effective = defaults_;
        for (const auto& override_argument : overrides)
        {
            const auto& name     = override_argument.first;
            const auto  position = std::find_if(effective.begin(), effective.end(),
                                                [&name](const Argument& argument)
                                                {
                                                   return argument.first == name;
                                               });
            if (position == effective.end())
            {
                throw std::invalid_argument("Unknown argument: " + name);
            }
            position->second = override_argument.second;
        }

        auto bound = bind_arguments(effective, Indices{});
        return std::apply(
            [this](const auto&... values)
            {
                return (object_->*method_)(values...);
            },
            bound);
    }

  private:
    void validate_names() const
    {
        std::unordered_set<std::string> names;
        for (const auto& argument : defaults_)
        {
            if (argument.first.empty())
            {
                throw std::invalid_argument("Argument name must not be empty");
            }
            if (!names.insert(argument.first).second)
            {
                throw std::invalid_argument("Duplicate argument: " + argument.first);
            }
        }
    }

    template <std::size_t Index>
    static decltype(auto) bind_named_argument(const ArgumentList& arguments)
    {
        using Value          = std::decay_t<std::tuple_element_t<Index, Parameters>>;
        const auto& argument = arguments[Index];
        try
        {
            return detail::bind_argument<Value>(argument.second);
        }
        catch (const std::invalid_argument&)
        {
            throw std::invalid_argument("Invalid type for argument: " + argument.first);
        }
    }

    template <std::size_t... Index>
    static auto bind_arguments(const ArgumentList&                            arguments,
                               [[maybe_unused]] std::index_sequence<Index...> indices)
    {
        return std::tuple<decltype(bind_named_argument<Index>(arguments))...>{
            bind_named_argument<Index>(arguments)...};
    }

    Object*      object_;
    Method       method_;
    ArgumentList defaults_;
};

#endif // WRAPPER_WRAPPER_H
