#include "arguments.h"
#include "test_support.h"

#include <any>
#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace
{

using test_support::require;
using test_support::require_throws;

struct CustomArgument
{
    std::string label;
    int         revision = 0;
};

void preserves_integer_value()
{
    constexpr int       expected = 4;
    const ArgumentValue value(expected);

    require(value.as<int>() == expected, "Integer value must be preserved");
}

void rejects_mismatched_type()
{
    const ArgumentValue value(0);

    require_throws<std::invalid_argument>(
        [&value]
        {
            (void)value.as<double>();
        });
    require_throws<std::invalid_argument>(
        [&value]
        {
            (void)value.as<std::string>();
        });
}

void owns_string_copy()
{
    std::string         source = "text";
    const ArgumentValue value(source);
    source.assign("changed");

    require(value.as<std::string>() == "text", "String must not alias its source");
}

void normalizes_string_literal()
{
    const ArgumentValue value("text");

    require(value.as<std::string>() == "text", "Literal must be stored as std::string");
}

void owns_mutable_c_string()
{
    std::array          source = {'t', 'e', 'x', 't', '\0'};
    const ArgumentValue value(source.data());
    source.front() = 'n';

    require(value.as<std::string>() == "text", "Mutable C string must be copied");
}

void owns_const_c_string()
{
    std::string         source  = "text";
    const char*         pointer = source.c_str();
    const ArgumentValue value(pointer);
    source.front() = 'n';

    require(value.as<std::string>() == "text", "Const C string must be copied");
}

void rejects_null_c_strings()
{
    const char* const null_const   = nullptr;
    char* const       null_mutable = nullptr;

    require_throws<std::invalid_argument>(
        [null_const]
        {
            (void)ArgumentValue(null_const);
        });
    require_throws<std::invalid_argument>(
        [null_mutable]
        {
            (void)ArgumentValue(null_mutable);
        });
}

void owns_temporary_string()
{
    const ArgumentValue value{std::string("text")};

    require(value.as<std::string>() == "text", "Temporary string must remain available");
}

void owns_string_view_after_source_destruction()
{
    const ArgumentValue value = []
    {
        const std::string source = "text";
        return ArgumentValue{std::string_view(source)};
    }();

    require(value.as<std::string>() == "text", "String view must be copied before source dies");
}

void preserves_string_view_length()
{
    constexpr std::size_t length = 3;
    const ArgumentValue   value{std::string_view("a\0b", length)};

    require(value.as<std::string>() == std::string("a\0b", length),
            "String view must preserve embedded null bytes");
}

void accepts_empty_string_view()
{
    const ArgumentValue value{std::string_view{}};

    require(value.as<std::string>().empty(), "Empty string view must become an empty string");
}

void owns_custom_copyable_type()
{
    constexpr int       revision = 7;
    CustomArgument      source{"original", revision};
    const ArgumentValue value(source);
    source.label.assign("changed");
    source.revision = 0;

    const auto& stored = value.as<CustomArgument>();
    require(stored.label == "original", "Custom argument must not alias its source");
    require(stored.revision == revision, "Custom argument fields must be preserved");
}

void preserves_any_as_argument_type()
{
    constexpr int       expected = 4;
    const std::any      source(expected);
    const ArgumentValue value(source);

    require(std::any_cast<int>(value.as<std::any>()) == expected,
            "A std::any argument must preserve its own type");
}

void copies_and_moves_argument_values()
{
    ArgumentValue       original("text");
    ArgumentValue       copy(original);
    const ArgumentValue moved(std::move(copy));

    require(original.as<std::string>() == "text", "Copy must preserve original value");
    require(moved.as<std::string>() == "text", "Copy and move must preserve contained type");
}

void initializes_named_argument_containers()
{
    constexpr int      expected  = 4;
    const ArgumentList defaults  = {{"arg1", expected}, {"arg2", "text"}};
    const ArgumentMap  overrides = {{"arg2", "replacement"}};

    require(defaults.front().first == "arg1", "Defaults must preserve parameter order");
    require(defaults.front().second.as<int>() == expected, "Named integer must be available");
    require(defaults.back().second.as<std::string>() == "text", "Named literal must be normalized");
    require(overrides.at("arg2").as<std::string>() == "replacement",
            "Overrides must support named values");
}

static_assert(std::is_copy_constructible_v<ArgumentValue>);
static_assert(std::is_move_constructible_v<ArgumentValue>);
static_assert(!std::is_constructible_v<ArgumentValue, std::unique_ptr<int>>);

} // namespace

int main()
{
    try
    {
        return test_support::run({
            {"preserves_integer_value", preserves_integer_value},
            {"rejects_mismatched_type", rejects_mismatched_type},
            {"owns_string_copy", owns_string_copy},
            {"normalizes_string_literal", normalizes_string_literal},
            {"owns_mutable_c_string", owns_mutable_c_string},
            {"owns_const_c_string", owns_const_c_string},
            {"rejects_null_c_strings", rejects_null_c_strings},
            {"owns_temporary_string", owns_temporary_string},
            {
                "owns_string_view_after_source_destruction",
                owns_string_view_after_source_destruction,
            },
            {"preserves_string_view_length", preserves_string_view_length},
            {"accepts_empty_string_view", accepts_empty_string_view},
            {"owns_custom_copyable_type", owns_custom_copyable_type},
            {"preserves_any_as_argument_type", preserves_any_as_argument_type},
            {"copies_and_moves_argument_values", copies_and_moves_argument_values},
            {"initializes_named_argument_containers", initializes_named_argument_containers},
        });
    }
    catch (...)
    {
        return 1;
    }
}
