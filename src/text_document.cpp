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

    return text_.find(phrase) != std::string::npos ? "found" : "not found";
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
        for (std::size_t index = range.first; index < range.second; ++index)
        {
            if (text::is_letter(value[index]))
            {
                value[index] = text::uppercase(value[index]);
                break;
            }
        }
    }

    return update_text(value);
}

std::string TextDocument::update_text(const std::u32string& value)
{
    std::string updated = text::encode_utf8(value);
    std::string result  = updated;
    text_.swap(updated);
    return result;
}
