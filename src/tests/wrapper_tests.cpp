#include "test_support.h"
#include "wrapper.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace
{

using test_support::require;
using test_support::require_throws;

struct CustomArgument
{
    std::string label;
};

struct CopyOnlyArgument
{
    explicit CopyOnlyArgument(std::string value) : label(std::move(value))
    {
    }

    CopyOnlyArgument(const CopyOnlyArgument&)            = default;
    CopyOnlyArgument& operator=(const CopyOnlyArgument&) = default;
    CopyOnlyArgument(CopyOnlyArgument&&)                 = delete;
    CopyOnlyArgument& operator=(CopyOnlyArgument&&)      = delete;

    std::string label;
};

class Subject
{
  public:
    std::string sum(int lhs, int rhs)
    {
        ++calls;
        return std::to_string(lhs + rhs);
    }

    std::string difference(int lhs, int rhs)
    {
        ++calls;
        return std::to_string(lhs - rhs);
    }

    // NOLINTBEGIN(modernize-use-string-view) -- These fixtures test the owning string result
    // contract.
    std::string zero()
    {
        ++calls;
        return "zero";
    }

    std::string constant() const
    {
        ++calls;
        return "const";
    }

    std::string stable() noexcept
    {
        ++calls;
        return {};
    }

    std::string stable_constant() const noexcept
    {
        ++calls;
        return {};
    }
    // NOLINTEND(modernize-use-string-view)

    std::string mixed(int number, const std::string& prefix, CustomArgument payload)
    {
        ++calls;
        return std::to_string(number) + ":" + prefix + ":" + std::move(payload.label);
    }

    template <typename... Values> std::string sum_many(Values... values)
    {
        ++calls;
        return std::to_string((values + ...));
    }

    std::string view(std::string_view text)
    {
        ++calls;
        return std::string(text);
    }

    std::string view_reference(const std::string_view& text)
    {
        ++calls;
        return std::string(text);
    }

    std::string c_string(const char* text)
    {
        ++calls;
        return {text};
    }

    std::string copy_only(CopyOnlyArgument payload)
    {
        ++calls;
        return std::move(payload.label);
    }

    std::string failing()
    {
        ++calls;
        throw std::runtime_error("Subject failure");
    }

    mutable int calls = 0;
};

template <typename Callable>
void require_argument_error(Callable&& callable, const std::string& name)
{
    try
    {
        std::forward<Callable>(callable)();
    }
    catch (const std::invalid_argument& error)
    {
        const std::string message(error.what());
        const auto found = std::search(message.begin(), message.end(), name.begin(), name.end(),
                                       [](char left, char right)
                                       {
                                           return left == right;
                                       });
        require(found != message.end(), "Argument error must include the offending name");
        return;
    }

    throw std::runtime_error("Expected an argument error");
}

void applies_defaults_and_partial_overrides()
{
    constexpr int replacement = 8;
    Subject       subject;
    Wrapper       wrapper(&subject, &Subject::sum, {{"lhs", 2}, {"rhs", 3}});

    require(wrapper.execute() == "5", "Defaults must be used without overrides");
    require(wrapper.execute({{"rhs", replacement}}) == "10",
            "Partial override must preserve other defaults");
    require(wrapper.execute() == "5", "Overrides must not mutate defaults");
    require(wrapper.execute({{"rhs", replacement}, {"lhs", 4}}) == "12",
            "All overrides must be applied");
}

void preserves_named_parameter_order()
{
    constexpr int replacement = 8;
    Subject       subject;
    Wrapper       wrapper(&subject, &Subject::difference, {{"zeta", 4}, {"alpha", 2}});

    require(wrapper.execute() == "2", "Defaults must bind by list order rather than name order");
    require(wrapper.execute({{"alpha", replacement}, {"zeta", 4}}) == "-4",
            "Override dictionary order must not change positional arguments");
    require(wrapper.execute({{"zeta", 4}, {"alpha", replacement}}) == "-4",
            "Reversing override insertion order must preserve positional binding");
}

void calls_original_object_through_command_interface()
{
    Subject         subject;
    Wrapper         wrapper(&subject, &Subject::zero, {});
    const ICommand& command = wrapper;

    require(command.execute() == "zero", "Command interface must invoke zero-arity methods");
    require(subject.calls == 1, "Wrapper must invoke the original object");
}

void supports_const_methods_on_const_objects()
{
    const Subject subject;
    Wrapper       wrapper(&subject, &Subject::constant, {});

    require(wrapper.execute() == "const", "Const methods must accept const objects");
    require(subject.calls == 1, "Const method must execute once");
}

void supports_noexcept_methods()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::stable, {});

    require(wrapper.execute().empty(), "Noexcept methods must be supported");
    require(subject.calls == 1, "Noexcept method must execute once");
}

void supports_const_noexcept_methods()
{
    const Subject subject;
    Wrapper       wrapper(&subject, &Subject::stable_constant, {});

    require(wrapper.execute().empty(), "Const noexcept methods must be supported");
    require(subject.calls == 1, "Const noexcept method must execute once");
}

void supports_mixed_argument_types()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::mixed,
                    {{"number", 4}, {"prefix", "item"}, {"payload", CustomArgument{"custom"}}});

    require(wrapper.execute() == "4:item:custom", "Mixed values and const references must bind");
    require(wrapper.execute({{"payload", CustomArgument{"new"}}}) == "4:item:new",
            "Custom argument overrides must preserve their type");
}

void supports_six_arguments()
{
    constexpr int fifth = 5;
    constexpr int sixth = 6;
    Subject       subject;
    ArgumentList  defaults = {
        {"first", 1},  {"second", 2},    {"third", 3},
        {"fourth", 4}, {"fifth", fifth}, {"sixth", sixth},
    };
    Wrapper wrapper(&subject, &Subject::sum_many<int, int, int, int, int, int>,
                    std::move(defaults));

    require(wrapper.execute() == "21", "Arity must not be restricted to a fixed overload set");
}

void adapts_owned_strings_to_views()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::view, {{"text", std::string("text")}});

    require(wrapper.execute() == "text", "View must refer to owned default storage");
    require(wrapper.execute({{"text", std::string_view("replacement")}}) == "replacement",
            "View override must refer to owned effective storage");
}

void adapts_owned_strings_to_view_references()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::view_reference, {{"text", "text"}});

    require(wrapper.execute() == "text", "Reference to a bound view must live through the call");
    require(wrapper.execute({{"text", std::string("replacement")}}) == "replacement",
            "Temporary string override must remain valid during the call");
}

void adapts_owned_strings_to_c_strings()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::c_string, {{"text", "text"}});

    require(wrapper.execute() == "text", "C string must refer to owned default storage");
    require(wrapper.execute({{"text", std::string("replacement")}}) == "replacement",
            "C string override must remain valid during the call");
}

void supports_copy_only_arguments()
{
    Subject                subject;
    const CopyOnlyArgument payload("copy only");
    Wrapper                wrapper(&subject, &Subject::copy_only, {{"payload", payload}});

    require(wrapper.execute() == "copy only", "Copyable arguments need not be movable");
}

void owns_default_values()
{
    Subject      subject;
    ArgumentList defaults = {{"text", "original"}};
    Wrapper      wrapper(&subject, &Subject::view, defaults);
    defaults.front().second = ArgumentValue("changed");

    require(wrapper.execute() == "original", "Wrapper must own its defaults independently");
}

void rejects_null_object()
{
    require_throws<std::invalid_argument>(
        []
        {
            (void)Wrapper(static_cast<Subject*>(nullptr), &Subject::sum, {{"lhs", 2}, {"rhs", 3}});
        });
}

void rejects_null_method()
{
    Subject                       subject;
    const decltype(&Subject::sum) method = nullptr;

    require_throws<std::invalid_argument>(
        [&subject, method]
        {
            (void)Wrapper(&subject, method, {{"lhs", 2}, {"rhs", 3}});
        });
    require(subject.calls == 0, "Invalid wrapper must not invoke any method");
}

void rejects_empty_and_duplicate_names()
{
    Subject subject;
    require_throws<std::invalid_argument>(
        [&subject]
        {
            (void)Wrapper(&subject, &Subject::sum, {{"", 2}, {"rhs", 3}});
        });
    require_throws<std::invalid_argument>(
        [&subject]
        {
            (void)Wrapper(&subject, &Subject::sum, {{"same", 2}, {"same", 3}});
        });
    require(subject.calls == 0, "Name validation must not invoke the method");
}

void rejects_incorrect_default_count()
{
    Subject subject;
    require_throws<std::invalid_argument>(
        [&subject]
        {
            (void)Wrapper(&subject, &Subject::sum, {{"lhs", 2}});
        });
    require_throws<std::invalid_argument>(
        [&subject]
        {
            (void)Wrapper(&subject, &Subject::sum, {{"lhs", 2}, {"rhs", 3}, {"extra", 4}});
        });
    require(subject.calls == 0, "Arity validation must not invoke the method");
}

void rejects_incorrect_default_type()
{
    Subject subject;
    require_argument_error(
        [&subject]
        {
            (void)Wrapper(&subject, &Subject::sum, {{"lhs", 2}, {"rhs", "wrong"}});
        },
        "rhs");
    require(subject.calls == 0, "Default type validation must not invoke the method");
}

void rejects_unknown_override()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::sum, {{"lhs", 2}, {"rhs", 3}});

    require_argument_error(
        [&wrapper]
        {
            (void)wrapper.execute({{"lhs", 4}, {"unknown", 0}});
        },
        "unknown");
    require(subject.calls == 0, "Unknown argument must prevent invocation");
    require(wrapper.execute() == "5", "Failed call must preserve defaults");
}

void rejects_incorrect_override_type()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::sum, {{"lhs", 2}, {"rhs", 3}});

    require_argument_error(
        [&wrapper]
        {
            (void)wrapper.execute({{"lhs", 4}, {"rhs", "wrong"}});
        },
        "rhs");
    require(subject.calls == 0, "All types must be validated before invocation");
    require(wrapper.execute() == "5", "Type errors must preserve defaults");
}

void propagates_method_exceptions()
{
    Subject subject;
    Wrapper wrapper(&subject, &Subject::failing, {});

    require_throws<std::runtime_error>(
        [&wrapper]
        {
            (void)wrapper.execute();
        });
    require(subject.calls == 1, "Method exceptions must propagate after one invocation");
}

} // namespace

int main()
{
    try
    {
        return test_support::run({
            {"applies_defaults_and_partial_overrides", applies_defaults_and_partial_overrides},
            {"preserves_named_parameter_order", preserves_named_parameter_order},
            {
                "calls_original_object_through_command_interface",
                calls_original_object_through_command_interface,
            },
            {"supports_const_methods_on_const_objects", supports_const_methods_on_const_objects},
            {"supports_noexcept_methods", supports_noexcept_methods},
            {"supports_const_noexcept_methods", supports_const_noexcept_methods},
            {"supports_mixed_argument_types", supports_mixed_argument_types},
            {"supports_six_arguments", supports_six_arguments},
            {"adapts_owned_strings_to_views", adapts_owned_strings_to_views},
            {"adapts_owned_strings_to_view_references", adapts_owned_strings_to_view_references},
            {"adapts_owned_strings_to_c_strings", adapts_owned_strings_to_c_strings},
            {"supports_copy_only_arguments", supports_copy_only_arguments},
            {"owns_default_values", owns_default_values},
            {"rejects_null_object", rejects_null_object},
            {"rejects_null_method", rejects_null_method},
            {"rejects_empty_and_duplicate_names", rejects_empty_and_duplicate_names},
            {"rejects_incorrect_default_count", rejects_incorrect_default_count},
            {"rejects_incorrect_default_type", rejects_incorrect_default_type},
            {"rejects_unknown_override", rejects_unknown_override},
            {"rejects_incorrect_override_type", rejects_incorrect_override_type},
            {"propagates_method_exceptions", propagates_method_exceptions},
        });
    }
    catch (...)
    {
        return 1;
    }
}
