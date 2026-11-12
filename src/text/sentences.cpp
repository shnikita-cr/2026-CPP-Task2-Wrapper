#include "sentences.h"

#include "utf8.h"

#include <string_view>

namespace
{

constexpr std::u32string_view ending_marks  = U".!?";
constexpr std::u32string_view closing_marks = U")]}»”’";

struct QuoteState
{
    bool double_open = false;
    bool single_open = false;
};

[[nodiscard]] bool is_ascii_whitespace(char32_t character) noexcept
{
    return character == U' ' || (character >= U'\t' && character <= U'\r');
}

[[nodiscard]] bool is_sentence_ending(char32_t character) noexcept
{
    return ending_marks.find(character) != std::u32string_view::npos;
}

[[nodiscard]] bool is_word_apostrophe(const std::u32string& value, std::size_t index,
                                      const QuoteState& quotes) noexcept
{
    return index > 0 && text::is_word_character(value[index - 1]) &&
           (!quotes.single_open ||
            (index + 1 < value.size() && text::is_word_character(value[index + 1])));
}

void observe_quote(const std::u32string& value, std::size_t index, QuoteState& quotes) noexcept
{
    if (value[index] == U'"')
    {
        quotes.double_open = !quotes.double_open;
    }
    else if (value[index] == U'\'' && !is_word_apostrophe(value, index, quotes))
    {
        quotes.single_open = !quotes.single_open;
    }
}

[[nodiscard]] bool is_sentence_suffix(char32_t character, const QuoteState& quotes) noexcept
{
    if (character == U'"')
    {
        return quotes.double_open;
    }
    if (character == U'\'')
    {
        return quotes.single_open;
    }

    return is_sentence_ending(character) ||
           closing_marks.find(character) != std::u32string_view::npos;
}

void append_trimmed_range(const std::u32string& value, std::size_t start, std::size_t finish,
                          text::SentenceRanges& ranges)
{
    while (start < finish && is_ascii_whitespace(value[start]))
    {
        ++start;
    }
    while (finish > start && is_ascii_whitespace(value[finish - 1]))
    {
        --finish;
    }
    if (start < finish)
    {
        ranges.emplace_back(start, finish);
    }
}

} // namespace

text::SentenceRanges text::sentence_ranges(const std::u32string& value)
{
    SentenceRanges ranges;
    QuoteState     quotes;
    std::size_t    start  = 0;
    std::size_t    offset = 0;
    while (offset < value.size())
    {
        if (!is_sentence_ending(value[offset]))
        {
            observe_quote(value, offset, quotes);
            ++offset;
            continue;
        }

        ++offset;
        while (offset < value.size() && is_sentence_suffix(value[offset], quotes))
        {
            observe_quote(value, offset, quotes);
            ++offset;
        }
        append_trimmed_range(value, start, offset, ranges);
        start = offset;
    }
    append_trimmed_range(value, start, value.size(), ranges);
    return ranges;
}
