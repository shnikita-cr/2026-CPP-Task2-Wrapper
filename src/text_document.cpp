#include "text_document.h"

#include "text/sentences.h"
#include "text/utf8.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

TextDocument::TextDocument(std::string text) : text_(std::move(text))
{
    (void)text::decode_utf8(text_);
}

const std::string& TextDocument::text() const noexcept
{
    return text_;
}

std::string TextDocument::replace_words(const std::string& from,
                                        const std::string& replacement_text)
{
    const auto needle = text::decode_utf8(from);
    if (needle.empty() || !std::all_of(needle.begin(), needle.end(), text::is_word_character))
    {
        throw std::invalid_argument("Replacement source must be one nonempty word");
    }
    const auto replacement = text::decode_utf8(replacement_text);
    const auto original    = text::decode_utf8(text_);

    std::u32string replaced;
    std::size_t    offset = 0;
    while (offset < original.size())
    {
        if (!text::is_word_character(original[offset]))
        {
            replaced.push_back(original[offset]);
            ++offset;
            continue;
        }

        const auto start = offset;
        while (offset < original.size() && text::is_word_character(original[offset]))
        {
            ++offset;
        }
        if (original.compare(start, offset - start, needle) == 0)
        {
            replaced.append(replacement);
        }
        else
        {
            replaced.append(original, start, offset - start);
        }
    }

    return update_text(replaced);
}

std::string TextDocument::find_phrase(const std::string& phrase) const
{
    if (phrase.empty())
    {
        throw std::invalid_argument("Search phrase must not be empty");
    }
    (void)text::decode_utf8(phrase);

    const auto found = std::search(text_.begin(), text_.end(), phrase.begin(), phrase.end(),
                                   [](char left, char right)
                                   {
                                       return left == right;
                                   });
    return found != text_.end() ? "found" : "not found";
}

std::string TextDocument::capitalize_words()
{
    auto value   = text::decode_utf8(text_);
    bool pending = true;
    for (auto& character : value)
    {
        if (!text::is_word_character(character))
        {
            pending = true;
            continue;
        }
        if (pending && text::is_letter(character))
        {
            character = text::uppercase(character);
            pending   = false;
        }
    }

    return update_text(value);
}

std::string TextDocument::capitalize_sentences()
{
    auto       value  = text::decode_utf8(text_);
    const auto ranges = text::sentence_ranges(value);
    for (const auto& range : ranges)
    {
        const auto first =
            value.begin() + static_cast<std::u32string::difference_type>(range.first);
        const auto last =
            value.begin() + static_cast<std::u32string::difference_type>(range.second);
        const auto letter = std::find_if(first, last, text::is_letter);
        if (letter != last)
        {
            *letter = text::uppercase(*letter);
        }
    }

    return update_text(value);
}

std::string TextDocument::number_sentences()
{
    const auto     value  = text::decode_utf8(text_);
    const auto     ranges = text::sentence_ranges(value);
    std::u32string numbered;
    std::size_t    ordinal = 1;
    for (const auto& range : ranges)
    {
        if (!numbered.empty())
        {
            numbered.push_back(U'\n');
        }
        const auto number = std::to_string(ordinal);
        numbered.append(number.begin(), number.end());
        numbered.append(U". ");
        numbered.append(value, range.first, range.second - range.first);
        ++ordinal;
    }

    return update_text(numbered);
}

std::string TextDocument::update_text(const std::u32string& value)
{
    std::string updated = text::encode_utf8(value);
    std::string result  = updated;
    text_.swap(updated);
    return result;
}
