#include "test_support.h"
#include "text_document.h"

#include <array>
#include <stdexcept>
#include <string>

namespace
{

using test_support::require;
using test_support::require_throws;

void preserves_initial_utf8_text()
{
    const TextDocument document(u8"hello, ёж 🐈!");

    require(document.text() == u8"hello, ёж 🐈!", "Document must preserve valid UTF-8");
}

void replaces_whole_words()
{
    TextDocument document("cat scatter cat.");

    require(document.replace_words("cat", "dog") == "dog scatter dog.",
            "Replacement must affect whole words only");
    require(document.text() == "dog scatter dog.", "Replacement must update document state");
}

void does_not_reprocess_inserted_words()
{
    TextDocument document("cat");

    require(document.replace_words("cat", "catcat") == "catcat",
            "Inserted text must not be replaced again");
}

void allows_empty_replacement()
{
    TextDocument document("cat cat.");

    require(document.replace_words("cat", "") == " .",
            "Empty replacement must preserve surrounding whitespace and punctuation");
}

void respects_word_boundaries_and_case()
{
    TextDocument document("cat cat_1 1cat scatter Cat cat-cat cat's");

    require(document.replace_words("cat", "dog") == "dog cat_1 1cat scatter Cat dog-dog dog's",
            "Digits and underscores belong to words; hyphens and apostrophes delimit them");
}

void replaces_russian_words()
{
    TextDocument document(u8"кот, котик и кот.");

    require(document.replace_words(u8"кот", u8"пёс") == u8"пёс, котик и пёс.",
            "Russian replacement must preserve word boundaries and UTF-8");
}

void allows_arbitrary_utf8_replacement()
{
    TextDocument document("cat!");

    require(document.replace_words("cat", u8"kind 🐈") == u8"kind 🐈!",
            "Replacement may contain multiple words and emoji");
}

void finds_exact_case_sensitive_phrases_without_mutation()
{
    const TextDocument document("hello world!");

    require(document.find_phrase("hello world") == "found", "Exact phrase must be found");
    require(document.find_phrase("Hello world") == "not found", "Search must be case sensitive");
    require(document.find_phrase("ello wor") == "found", "Search must allow exact substrings");
    require(document.text() == "hello world!", "Search must not mutate the document");
}

void finds_utf8_phrases()
{
    const TextDocument document(u8"ёж идёт. кот спит.");

    require(document.find_phrase(u8"ёж идёт") == "found", "Russian phrase must be found");
    require(document.find_phrase(u8"кот идёт") == "not found", "Missing phrase must not be found");
}

void rejects_invalid_search_queries()
{
    const TextDocument document("original");
    const std::array   invalid = {"", "\xC0\xAF"};
    for (const auto* query : invalid)
    {
        require_throws<std::invalid_argument>(
            [&document, query]
            {
                (void)document.find_phrase(query);
            });
    }

    require(document.text() == "original", "Invalid search must preserve the document");
}

void rejects_invalid_replacement_sources()
{
    TextDocument     document("original");
    const std::array invalid = {"", "two words", "bad!", "\xC0\xAF"};
    for (const auto* source : invalid)
    {
        require_throws<std::invalid_argument>(
            [&document, source]
            {
                (void)document.replace_words(source, "new");
            });
    }

    require(document.text() == "original", "Invalid source must preserve the document");
}

void rejects_invalid_replacement_value()
{
    TextDocument document("cat");

    require_throws<std::invalid_argument>(
        [&document]
        {
            (void)document.replace_words("cat", "\xED\xA0\x80");
        });
    require(document.text() == "cat", "Invalid replacement must preserve the document");
}

void rejects_invalid_document_utf8()
{
    require_throws<std::invalid_argument>(
        []
        {
            (void)TextDocument("\xF0\x90\x80");
        });
}

void handles_empty_document()
{
    TextDocument document("");

    require(document.replace_words("cat", "dog").empty(), "Empty document must remain empty");
    require(document.find_phrase("cat") == "not found", "Empty document cannot contain a phrase");
}

void accepts_arguments_aliased_to_document_storage()
{
    TextDocument document("cat dog");
    require(document.replace_words("cat", document.text()) == "cat dog dog",
            "Replacement may refer to the original document storage");

    TextDocument single_word("cat");
    require(single_word.replace_words(single_word.text(), "dog") == "dog",
            "Source may refer to the original document storage");
}

void preserves_utf8_without_matches()
{
    TextDocument document(u8"hello 🐈");

    require(document.replace_words("cat", "dog") == u8"hello 🐈",
            "Unmatched Unicode text must remain intact");
}

} // namespace

int main()
{
    try
    {
        return test_support::run({
            {"preserves_initial_utf8_text", preserves_initial_utf8_text},
            {"replaces_whole_words", replaces_whole_words},
            {"does_not_reprocess_inserted_words", does_not_reprocess_inserted_words},
            {"allows_empty_replacement", allows_empty_replacement},
            {"respects_word_boundaries_and_case", respects_word_boundaries_and_case},
            {"replaces_russian_words", replaces_russian_words},
            {"allows_arbitrary_utf8_replacement", allows_arbitrary_utf8_replacement},
            {"finds_exact_case_sensitive_phrases_without_mutation",
             finds_exact_case_sensitive_phrases_without_mutation},
            {"finds_utf8_phrases", finds_utf8_phrases},
            {"rejects_invalid_search_queries", rejects_invalid_search_queries},
            {"rejects_invalid_replacement_sources", rejects_invalid_replacement_sources},
            {"rejects_invalid_replacement_value", rejects_invalid_replacement_value},
            {"rejects_invalid_document_utf8", rejects_invalid_document_utf8},
            {"handles_empty_document", handles_empty_document},
            {"accepts_arguments_aliased_to_document_storage",
             accepts_arguments_aliased_to_document_storage},
            {"preserves_utf8_without_matches", preserves_utf8_without_matches},
        });
    }
    catch (...)
    {
        return 1;
    }
}
