#ifndef WRAPPER_ARGUMENTS_H
#define WRAPPER_ARGUMENTS_H

#include <any>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

class ArgumentValue
{
  public:
    ArgumentValue(std::string value) : value_(std::move(value))
    {
    }

    ArgumentValue(std::string_view value) : value_(std::string(value))
    {
    }

    ArgumentValue(const char* value) : value_(copy_c_string(value))
    {
    }

    template <typename T,
              std::enable_if_t<
                  std::is_copy_constructible_v<T> && !std::is_same_v<T, ArgumentValue> &&
                      !std::is_same_v<T, std::string> && !std::is_same_v<T, std::string_view> &&
                      !std::is_same_v<T, char*> && !std::is_same_v<T, const char*>,
                  int> = 0>
    ArgumentValue(T value) : value_(std::in_place_type<T>, std::move_if_noexcept(value))
    {
    }

    template <typename T> [[nodiscard]] const T& as() const
    {
        const auto* stored = std::any_cast<T>(&value_);
        if (stored == nullptr)
        {
            throw std::invalid_argument("Argument type mismatch");
        }

        return *stored;
    }

  private:
    static std::string copy_c_string(const char* value)
    {
        if (value == nullptr)
        {
            throw std::invalid_argument("Null C string argument");
        }

        return {value};
    }

    std::any value_;
};

using Argument     = std::pair<std::string, ArgumentValue>;
using ArgumentList = std::vector<Argument>;
using ArgumentMap  = std::unordered_map<std::string, ArgumentValue>;

#endif // WRAPPER_ARGUMENTS_H
