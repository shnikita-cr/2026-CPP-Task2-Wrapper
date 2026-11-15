#include "engine.h"
#include "test_support.h"
#include "wrapper.h"

#include <stdexcept>
#include <string>

namespace
{

using test_support::require;
using test_support::require_throws;

class SubjectFailure : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

class NumberSubject
{
  public:
    std::string sum(int lhs, int rhs)
    {
        ++calls;
        return std::to_string(lhs + rhs);
    }

    int calls = 0;
};

class TextSubject
{
  public:
    std::string greet(const std::string& name)
    {
        ++calls;
        return "Hello, " + name;
    }

    std::string failing()
    {
        ++calls;
        throw SubjectFailure("Subject failure");
    }

    int calls = 0;
};

void routes_aliases_to_different_subjects()
{
    NumberSubject number;
    TextSubject   text;
    Wrapper       number_wrapper(&number, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Wrapper       text_wrapper(&text, &TextSubject::greet, {{"name", "world"}});
    Engine        engine;
    engine.register_command(&number_wrapper, "sum");
    engine.register_command(&text_wrapper, "greet");
    const Engine& registered = engine;

    require(registered.execute("sum") == "5", "Numeric alias must use its defaults");
    require(registered.execute("greet") == "Hello, world", "Text alias must use its defaults");
    require(registered.execute("sum", {{"rhs", 4}}) == "6", "Numeric overrides must be forwarded");
    require(registered.execute("greet", {{"name", "reader"}}) == "Hello, reader",
            "String overrides must be forwarded to the text subject");
    require(number.calls == 2, "Numeric alias must invoke only the numeric subject");
    require(text.calls == 2, "Text alias must invoke only the text subject");
}

void preserves_defaults_after_execution()
{
    constexpr int replacement = 8;
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine        engine;
    engine.register_command(&wrapper, "sum");

    require(engine.execute("sum", {{"rhs", replacement}}) == "10", "Override must apply once");
    require(engine.execute("sum") == "5", "Engine must not retain previous overrides");
}

void rejects_unknown_alias_without_reserving_it()
{
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine        engine;

    require_throws<std::out_of_range>(
        [&engine]
        {
            (void)engine.execute("later");
        });
    require(subject.calls == 0, "Unknown alias must not invoke a subject");
    engine.register_command(&wrapper, "later");
    require(engine.execute("later") == "5", "Failed lookup must not reserve an alias");
}

void rejects_null_command_without_reserving_alias()
{
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine        engine;

    require_throws<std::invalid_argument>(
        [&engine]
        {
            engine.register_command(nullptr, "sum");
        });
    engine.register_command(&wrapper, "sum");
    require(engine.execute("sum") == "5", "Invalid registration must not reserve an alias");
}

void rejects_empty_alias()
{
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine        engine;

    require_throws<std::invalid_argument>(
        [&engine, &wrapper]
        {
            engine.register_command(&wrapper, "");
        });
    require_throws<std::out_of_range>(
        [&engine]
        {
            (void)engine.execute("");
        });
    require(subject.calls == 0, "Empty alias must never invoke a command");
}

void duplicate_registration_preserves_original_command()
{
    NumberSubject original;
    NumberSubject replacement;
    Wrapper       original_wrapper(&original, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Wrapper       replacement_wrapper(&replacement, &NumberSubject::sum, {{"lhs", 1}, {"rhs", 1}});
    Engine        engine;
    engine.register_command(&original_wrapper, "sum");

    require_throws<std::invalid_argument>(
        [&engine, &replacement_wrapper]
        {
            engine.register_command(&replacement_wrapper, "sum");
        });
    require(engine.execute("sum") == "5", "Duplicate must not replace the original command");
    require(original.calls == 1, "Original command must remain registered");
    require(replacement.calls == 0, "Rejected command must not be invoked");
}

void propagates_original_method_exception()
{
    TextSubject subject;
    Wrapper     wrapper(&subject, &TextSubject::failing, {});
    Engine      engine;
    engine.register_command(&wrapper, "fail");

    require_throws<SubjectFailure>(
        [&engine]
        {
            (void)engine.execute("fail");
        });
    require(subject.calls == 1, "Original exception must propagate after one invocation");
}

void propagates_argument_errors_before_invocation()
{
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine        engine;
    engine.register_command(&wrapper, "sum");

    require_throws<std::invalid_argument>(
        [&engine]
        {
            (void)engine.execute("sum", {{"rhs", "wrong"}});
        });
    require_throws<std::invalid_argument>(
        [&engine]
        {
            (void)engine.execute("sum", {{"unknown", 0}});
        });
    require(subject.calls == 0, "Invalid arguments must prevent method invocation");
    require(engine.execute("sum") == "5", "Argument errors must preserve registered defaults");
}

void owns_alias_name()
{
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine        engine;
    std::string   alias = "original";
    engine.register_command(&wrapper, alias);
    alias.assign("changed");

    require(engine.execute("original") == "5", "Registered alias must be copied");
    require_throws<std::out_of_range>(
        [&engine, &alias]
        {
            (void)engine.execute(alias);
        });
}

void supports_multiple_aliases_for_one_command()
{
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine        engine;
    engine.register_command(&wrapper, "first");
    engine.register_command(&wrapper, "second");

    require(engine.execute("first", {{"rhs", 4}}) == "6", "First alias must forward overrides");
    require(engine.execute("second") == "5", "Second alias must use the command defaults");
}

void does_not_own_registered_commands()
{
    NumberSubject subject;
    Wrapper       wrapper(&subject, &NumberSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    {
        Engine engine;
        engine.register_command(&wrapper, "sum");
        require(engine.execute("sum") == "5", "Registered command must be callable");
    }

    require(wrapper.execute() == "5", "Command must remain valid after Engine destruction");
    require(subject.calls == 2, "Engine must borrow the original object and command");
}

} // namespace

int main()
{
    try
    {
        return test_support::run({
            {"routes_aliases_to_different_subjects", routes_aliases_to_different_subjects},
            {"preserves_defaults_after_execution", preserves_defaults_after_execution},
            {
                "rejects_unknown_alias_without_reserving_it",
                rejects_unknown_alias_without_reserving_it,
            },
            {
                "rejects_null_command_without_reserving_alias",
                rejects_null_command_without_reserving_alias,
            },
            {"rejects_empty_alias", rejects_empty_alias},
            {
                "duplicate_registration_preserves_original_command",
                duplicate_registration_preserves_original_command,
            },
            {"propagates_original_method_exception", propagates_original_method_exception},
            {
                "propagates_argument_errors_before_invocation",
                propagates_argument_errors_before_invocation,
            },
            {"owns_alias_name", owns_alias_name},
            {
                "supports_multiple_aliases_for_one_command",
                supports_multiple_aliases_for_one_command,
            },
            {"does_not_own_registered_commands", does_not_own_registered_commands},
        });
    }
    catch (...)
    {
        return 1;
    }
}
