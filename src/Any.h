#pragma once

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include "decay.h"

class Any
{
public:
    Any() = default;

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Any(const char* strng) : held_(new Holder<std::string>(strng))
    {}

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    Any(char* strng) : held_(new Holder<std::string>(strng))
    {}

    template<typename T, typename = std::enable_if_t<!std::is_base_of_v<Any, decay_t<T>>>>
    explicit Any(T&& temp) : held_(new Holder<decay_t<T>>(std::forward<T>(temp)))
    {}

    Any(const Any& othr) : held_((othr.held_ != nullptr) ? othr.held_->clone() : nullptr)
    {}
    Any(Any&& othr) noexcept : held_(othr.held_)
    {
        othr.held_ = nullptr;
    }


    Any& operator=(const Any& othr)
    {
        if (this != &othr)
        {
            Any tmp(othr);
            swap(tmp);
        }
        return *this;
    }
    Any& operator=(Any&& othr) noexcept
    {
        if (this != &othr)
        {
            delete held_;
            held_ = othr.held_;
            othr.held_ = nullptr;
        }
        return *this;
    }

    ~Any()
    {
        delete held_;
    }

    void swap(Any& othr) noexcept
    {
        std::swap(held_, othr.held_);
    }

    [[nodiscard, maybe_unused]] bool has_value() const
    {
        return held_ != nullptr;
    }

    [[nodiscard, maybe_unused]] const std::type_info& type_info() const
    {
        return (held_ != nullptr) ? held_->type_info() : typeid(void);
    }

    void print(std::ostream& outs) const
    {
        if (held_ != nullptr)
        {
            held_->print(outs);
        }
        else
        {
            outs << "<empty>";
        }
    }

    template<typename U>
    [[nodiscard]] const decay_t<U>& get() const
    {
        using T = decay_t<U>;
        if (!held_ || typeid(T) != held_->type_info())
        {
            throw std::runtime_error("Bad any cast");
        }
        return static_cast<const Holder<T>*>(held_)->t_;
    }
private:
    template<typename T, typename = void>
    struct is_streamable : std::false_type
    {};

    template<typename T>
    struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>> : std::true_type
    {};

    template<typename T>
    static constexpr bool is_streamable_v = is_streamable<T>::value;

    struct Base
    {
        virtual ~Base() = default;
        [[nodiscard]] virtual const std::type_info& type_info() const = 0;
        [[nodiscard]] virtual Base* clone() const = 0;
        virtual void print(std::ostream&) const = 0;
    };

    template<typename T>
    struct Holder : Base
    {
        template<typename U, typename = std::enable_if_t<!std::is_base_of_v<Any, decay_t<U>>>>
        explicit Holder(U&& utype) : t_(std::forward<U>(utype))
        {}
        [[nodiscard]] const std::type_info& type_info() const override
        {
            return typeid(t_);
        }
        [[nodiscard]] Base* clone() const override
        {
            return new Holder<T>(t_);
        }

        void print(std::ostream& outs) const override
        {
            if constexpr (is_streamable_v<T>)
            {
                outs << t_;
            }
            else
            {
                outs << "<any>";
            }
        }

        T t_;
    };

    Base* held_ = nullptr;
};

inline std::ostream& operator<<(std::ostream& outs, const Any& any)
{
    any.print(outs);
    return outs;
}