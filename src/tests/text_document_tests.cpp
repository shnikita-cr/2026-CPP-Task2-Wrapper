#include "test_support.h"
#include "text/sentences.h"
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

void capitalizes_word_initials()
{
    TextDocument document(u8"hello, world! ёж идёт.");

    require(document.capitalize_words() == u8"Hello, World! Ёж Идёт.",
            "Each ASCII or Russian word must start with a capital letter");
    require(document.text() == u8"Hello, World! Ёж Идёт.", "Word capitalization must update state");
}

void capitalizes_only_first_letter_in_each_token()
{
    TextDocument document("hELLO 123hello _foo_bar foo2bar");

    require(document.capitalize_words() == "HELLO 123Hello _Foo_bar Foo2bar",
            "Digits and underscores must not split tokens or prevent finding the first letter");
}

void capitalizes_words_separated_by_hyphens_and_emoji()
{
    TextDocument document(u8"hello-world🐈ёж");

    require(document.capitalize_words() == u8"Hello-World🐈Ёж",
            "Hyphens and emoji must delimit words without being changed");
}

void preserves_empty_punctuation_and_capitalized_words()
{
    const std::array samples = {"", " \t\r\n", u8"?!...🐈", u8"Hello, World! Ёж."};
    for (const auto* original : samples)
    {
        TextDocument document(original);
        require(document.capitalize_words() == original,
                "Word capitalization must preserve empty, punctuation-only and capitalized text");
    }
}

void capitalizes_sentence_initials()
{
    TextDocument document(u8"hello world. ёж идёт! \"next?\" last");

    require(document.capitalize_sentences() == u8"Hello world. Ёж идёт! \"Next?\" Last",
            "Each sentence must start with a capital letter");
    require(document.text() == u8"Hello world. Ёж идёт! \"Next?\" Last",
            "Sentence capitalization must update state");
}

void capitalizes_sentences_without_changing_layout()
{
    TextDocument document(u8" \t\"123 hello\nworld...\" \r\nёж?!  42 next");

    require(document.capitalize_sentences() == u8" \t\"123 Hello\nworld...\" \r\nЁж?!  42 Next",
            "Quotes and digits must be skipped while original whitespace remains intact");
}

void does_not_treat_newlines_as_sentence_endings()
{
    TextDocument document("first line\nsecond line. next\nlast");

    require(document.capitalize_sentences() == "First line\nsecond line. Next\nlast",
            "A newline alone must not start a new sentence");
}

void capitalizes_after_nested_closing_marks()
{
    TextDocument document(u8"«(one!)» [two?] {three...} “four.” ‘five!’ tail");

    require(document.capitalize_sentences() == u8"«(One!)» [Two?] {Three...} “Four.” ‘Five!’ Tail",
            "Closing quotes and brackets must not hide the next sentence");
}

void preserves_empty_punctuation_and_capitalized_sentences()
{
    const std::array samples = {"", " \t\r\n", u8"?!...🐈", u8"Hello world. Ёж идёт!"};
    for (const auto* original : samples)
    {
        TextDocument document(original);
        require(document.capitalize_sentences() == original,
                "Sentence capitalization must preserve text without lowercase initials");
    }
}

void capitalization_is_idempotent()
{
    TextDocument words("hello world");
    require(words.capitalize_words() == "Hello World", "First capitalization must change words");
    require(words.capitalize_words() == "Hello World", "Repeated capitalization must be stable");

    TextDocument sentences("hello world. next");
    require(sentences.capitalize_sentences() == "Hello world. Next",
            "First capitalization must change sentences");
    require(sentences.capitalize_sentences() == "Hello world. Next",
            "Repeated sentence capitalization must be stable");
}

void finds_trimmed_unicode_sentence_ranges()
{
    const std::u32string original = U" \tёж?!\n\"next?\" last  ";
    const auto           ranges   = text::sentence_ranges(original);
    const std::array     expected = {U"ёж?!", U"\"next?\"", U"last"};

    require(ranges.size() == expected.size(), "Expected exactly three Unicode sentence ranges");
    std::size_t previous_end = 0;
    for (std::size_t index = 0; index < ranges.size(); ++index)
    {
        const auto& range = ranges[index];
        require(range.first >= previous_end && range.first < range.second &&
                    range.second <= original.size(),
                "Sentence ranges must be ordered nonempty half-open character ranges");
        require(original.substr(range.first, range.second - range.first) == expected[index],
                "Ranges must preserve sentence content and trim surrounding ASCII whitespace");
        previous_end = range.second;
    }
}

void handles_empty_and_unterminated_sentence_ranges()
{
    require(text::sentence_ranges(U"").empty(), "Empty text must produce no sentence ranges");
    require(text::sentence_ranges(U" \t\n\r\v\f").empty(), "ASCII whitespace must be ignored");

    const std::u32string original = U" \ttail without stop\n ";
    const auto           ranges   = text::sentence_ranges(original);
    require(ranges.size() == 1, "Unterminated nonempty tail must form one sentence");
    require(original.substr(ranges.front().first, ranges.front().second - ranges.front().first) ==
                U"tail without stop",
            "Unterminated tail must also be trimmed");
}

void keeps_terminal_groups_and_nested_closers_together()
{
    const std::u32string original = U" «(one?!)» two... tail ";
    const auto           ranges   = text::sentence_ranges(original);
    const std::array     expected = {U"«(one?!)»", U"two...", U"tail"};

    require(ranges.size() == expected.size(), "Terminal groups must not produce extra sentences");
    for (std::size_t index = 0; index < ranges.size(); ++index)
    {
        require(original.substr(ranges[index].first, ranges[index].second - ranges[index].first) ==
                    expected[index],
                "Nested closers must stay with their terminating punctuation");
    }
}

void distinguishes_closing_quotes_from_new_opening_quotes()
{
    const std::u32string original = U"\"one!\"?\"two.\"";
    const auto           ranges   = text::sentence_ranges(original);

    require(ranges.size() == 2, "Quote-adjacent sentences must remain separate");
    require(original.substr(ranges.front().first, ranges.front().second - ranges.front().first) ==
                U"\"one!\"?",
            "Closing quote and following terminal punctuation belong to the first sentence");
    require(original.substr(ranges.back().first, ranges.back().second - ranges.back().first) ==
                U"\"two.\"",
            "Opening quote of the next sentence must not be consumed as a closer");
}

void treats_internal_apostrophes_as_text()
{
    const std::u32string original = U"don't. 'go.' next";
    const auto           ranges   = text::sentence_ranges(original);
    const std::array     expected = {U"don't.", U"'go.'", U"next"};

    require(ranges.size() == expected.size(), "Contractions must not change quote nesting");
    for (std::size_t index = 0; index < ranges.size(); ++index)
    {
        require(original.substr(ranges[index].first, ranges[index].second - ranges[index].first) ==
                    expected[index],
                "Quoted sentence boundaries must survive internal apostrophes");
    }
}

void distinguishes_possessives_from_closing_quotes()
{
    struct Sample
    {
        const char32_t* input;
        const char32_t* first_sentence;
    };
    const std::array samples = {
        Sample{U"dogs' toys. 'go.'", U"dogs' toys."},
        Sample{U"'dogs' play. 'go.'", U"'dogs' play."},
    };
    for (const auto& sample : samples)
    {
        const std::u32string original = sample.input;
        const auto           ranges   = text::sentence_ranges(original);
        require(ranges.size() == 2, "Possessive apostrophes must not create phantom quotes");
        require(
            original.substr(ranges.front().first, ranges.front().second - ranges.front().first) ==
                sample.first_sentence,
            "Trailing possessives and genuine closing quotes must keep their own meaning");
        require(original.substr(ranges.back().first, ranges.back().second - ranges.back().first) ==
                    U"'go.'",
                "Following quoted sentence must include its closing quote");
    }
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
            {"capitalizes_word_initials", capitalizes_word_initials},
            {"capitalizes_only_first_letter_in_each_token",
             capitalizes_only_first_letter_in_each_token},
            {"capitalizes_words_separated_by_hyphens_and_emoji",
             capitalizes_words_separated_by_hyphens_and_emoji},
            {"preserves_empty_punctuation_and_capitalized_words",
             preserves_empty_punctuation_and_capitalized_words},
            {"capitalizes_sentence_initials", capitalizes_sentence_initials},
            {"capitalizes_sentences_without_changing_layout",
             capitalizes_sentences_without_changing_layout},
            {"does_not_treat_newlines_as_sentence_endings",
             does_not_treat_newlines_as_sentence_endings},
            {"capitalizes_after_nested_closing_marks", capitalizes_after_nested_closing_marks},
            {"preserves_empty_punctuation_and_capitalized_sentences",
             preserves_empty_punctuation_and_capitalized_sentences},
            {"capitalization_is_idempotent", capitalization_is_idempotent},
            {"finds_trimmed_unicode_sentence_ranges", finds_trimmed_unicode_sentence_ranges},
            {"handles_empty_and_unterminated_sentence_ranges",
             handles_empty_and_unterminated_sentence_ranges},
            {"keeps_terminal_groups_and_nested_closers_together",
             keeps_terminal_groups_and_nested_closers_together},
            {"distinguishes_closing_quotes_from_new_opening_quotes",
             distinguishes_closing_quotes_from_new_opening_quotes},
            {"treats_internal_apostrophes_as_text", treats_internal_apostrophes_as_text},
            {"distinguishes_possessives_from_closing_quotes",
             distinguishes_possessives_from_closing_quotes},
        });
    }
    catch (...)
    {
        return 1;
    }
}
