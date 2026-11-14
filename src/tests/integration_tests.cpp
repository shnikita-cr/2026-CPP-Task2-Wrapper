#include "engine.h"
#include "test_support.h"
#include "text_document.h"
#include "wrapper.h"

#include <stdexcept>
#include <string>

namespace
{

using test_support::require;
using test_support::require_throws;

void executes_document_pipeline()
{
    TextDocument document("cat sleeps. dog runs!");
    Wrapper      replace(&document, &TextDocument::replace_words, {{"from", "cat"}, {"to", "fox"}});
    Wrapper      capitalize(&document, &TextDocument::capitalize_sentences, {});
    Wrapper      number(&document, &TextDocument::number_sentences, {});
    Engine       engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&capitalize, "capitalize_sentences");
    engine.register_command(&number, "number_sentences");

    require(engine.execute("replace", {{"to", "wolf"}}) == "wolf sleeps. dog runs!",
            "Partial override must keep the default source word");
    require(engine.execute("capitalize_sentences") == "Wolf sleeps. Dog runs!",
            "Capitalization must operate on the document changed by the previous command");
    require(engine.execute("number_sentences") == "1. Wolf sleeps.\n2. Dog runs!",
            "Numbering must complete the registered command pipeline");
    require(document.text() == "1. Wolf sleeps.\n2. Dog runs!",
            "Pipeline result must be stored in the original document");
}

void preserves_defaults_when_document_is_reset()
{
    TextDocument document("cat sleeps. dog runs!");
    Wrapper      replace(&document, &TextDocument::replace_words, {{"from", "cat"}, {"to", "fox"}});
    Engine       engine;
    engine.register_command(&replace, "replace");

    require(engine.execute("replace", {{"to", "wolf"}}) == "wolf sleeps. dog runs!",
            "Override must be used for its own invocation");
    document = TextDocument("cat sleeps. dog runs!");
    require(engine.execute("replace") == "fox sleeps. dog runs!",
            "Resetting document content must not preserve a previous argument override");
}

void invalid_calls_preserve_document_and_command()
{
    TextDocument document("cat sleeps. dog runs!");
    Wrapper      replace(&document, &TextDocument::replace_words, {{"from", "cat"}, {"to", "fox"}});
    Engine       engine;
    engine.register_command(&replace, "replace");

    require_throws<std::invalid_argument>(
        [&engine]
        {
            (void)engine.execute("replace", {{"to", 0}});
        });
    require(document.text() == "cat sleeps. dog runs!", "Wrong type must not mutate the document");
    require_throws<std::invalid_argument>(
        [&engine]
        {
            (void)engine.execute("replace", {{"unknown", "value"}});
        });
    require(document.text() == "cat sleeps. dog runs!", "Unknown key must not mutate the document");
    require_throws<std::invalid_argument>(
        [&engine]
        {
            (void)engine.execute("replace", {{"to", "\xED\xA0\x80"}});
        });
    require(document.text() == "cat sleeps. dog runs!",
            "Subject validation failure must be atomic");
    require(engine.execute("replace") == "fox sleeps. dog runs!",
            "Rejected calls must leave the registered defaults usable");
}

class NumericSubject
{
  public:
    std::string sum(int lhs, int rhs)
    {
        ++calls;
        return std::to_string(lhs + rhs);
    }

    int calls = 0;
};

void mixes_all_document_operations_with_numeric_commands()
{
    TextDocument   document(u8"cat sleeps. ёж идёт!");
    NumericSubject numeric;
    Wrapper replace(&document, &TextDocument::replace_words, {{"from", "cat"}, {"to", "fox"}});
    Wrapper find(&document, &TextDocument::find_phrase, {{"phrase", "fox sleeps"}});
    Wrapper words(&document, &TextDocument::capitalize_words, {});
    Wrapper sentences(&document, &TextDocument::capitalize_sentences, {});
    Wrapper number(&document, &TextDocument::number_sentences, {});
    Wrapper sum(&numeric, &NumericSubject::sum, {{"lhs", 2}, {"rhs", 3}});
    Engine  engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&words, "words");
    engine.register_command(&sentences, "sentences");
    engine.register_command(&number, "number");
    engine.register_command(&sum, "sum");

    require(engine.execute("sum") == "5", "Numeric defaults must work beside text commands");
    require(engine.execute("replace") == u8"fox sleeps. ёж идёт!", "Replacement must keep UTF-8");
    require(engine.execute("find") == "found", "Search must see the modified document");
    require(engine.execute("find", {{"phrase", "missing"}}) == "not found",
            "Search override must remain independent from other commands");
    require(engine.execute("sentences") == u8"Fox sleeps. Ёж идёт!", "Sentence caps must dispatch");
    require(engine.execute("words") == u8"Fox Sleeps. Ёж Идёт!", "Word caps must dispatch");
    require(engine.execute("number") == u8"1. Fox Sleeps.\n2. Ёж Идёт!", "Numbering must dispatch");
    require(engine.execute("sum", {{"lhs", 4}}) == "7", "Numeric overrides must remain usable");
    require(numeric.calls == 2, "Only numeric aliases must invoke the numeric subject");
}

} // namespace

int main()
{
    try
    {
        return test_support::run({
            {"executes_document_pipeline", executes_document_pipeline},
            {
                "preserves_defaults_when_document_is_reset",
                preserves_defaults_when_document_is_reset,
            },
            {
                "invalid_calls_preserve_document_and_command",
                invalid_calls_preserve_document_and_command,
            },
            {
                "mixes_all_document_operations_with_numeric_commands",
                mixes_all_document_operations_with_numeric_commands,
            },
        });
    }
    catch (...)
    {
        return 1;
    }
}
