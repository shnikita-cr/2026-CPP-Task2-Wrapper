#include "utf8.h"

#include <array>
#include <cstddef>
#include <stdexcept>

namespace
{

struct Encoding
{
    std::size_t length;
    char32_t    prefix;
    char32_t    mask;
    char32_t    minimum;
};

constexpr std::array encodings = {
    Encoding{1, 0x00, 0x7F, 0x00},
    Encoding{2, 0xC0, 0x1F, 0x80},
    Encoding{3, 0xE0, 0x0F, 0x800},
    Encoding{4, 0xF0, 0x07, 0x10000},
};
constexpr std::size_t continuation_bits   = 6;
constexpr char32_t    continuation_prefix = 0x80;
constexpr char32_t    continuation_mask   = 0x3F;
constexpr char32_t    maximum_scalar      = 0x10FFFF;
constexpr char32_t    first_surrogate     = 0xD800;
constexpr char32_t    last_surrogate      = 0xDFFF;

[[nodiscard]] bool is_scalar(char32_t character) noexcept
{
    return character <= maximum_scalar &&
           (character < first_surrogate || character > last_surrogate);
}

[[nodiscard]] const Encoding& encoding_for_lead(unsigned char lead)
{
    for (const auto& encoding : encodings)
    {
        if ((lead & ~encoding.mask) == encoding.prefix)
        {
            return encoding;
        }
    }

    throw std::invalid_argument("Invalid UTF-8 leading byte");
}

[[nodiscard]] char32_t read_character(const std::string& value, std::size_t& offset)
{
    const auto  lead     = static_cast<unsigned char>(value[offset]);
    const auto& encoding = encoding_for_lead(lead);
    if (encoding.length > value.size() - offset)
    {
        throw std::invalid_argument("Truncated UTF-8 sequence");
    }

    char32_t character = lead & encoding.mask;
    for (std::size_t index = 1; index < encoding.length; ++index)
    {
        const auto next = static_cast<unsigned char>(value[offset + index]);
        if ((next & ~continuation_mask) != continuation_prefix)
        {
            throw std::invalid_argument("Invalid UTF-8 continuation byte");
        }
        character = (character << continuation_bits) | (next & continuation_mask);
    }
    if (character < encoding.minimum || !is_scalar(character))
    {
        throw std::invalid_argument("Invalid UTF-8 scalar value");
    }

    offset += encoding.length;
    return character;
}

void append_character(std::string& output, char32_t character, const Encoding& encoding)
{
    std::size_t shift = continuation_bits * (encoding.length - 1);
    output.push_back(static_cast<char>(encoding.prefix | (character >> shift)));
    for (std::size_t index = 1; index < encoding.length; ++index)
    {
        shift           -= continuation_bits;
        const auto next  = continuation_prefix | ((character >> shift) & continuation_mask);
        output.push_back(static_cast<char>(next));
    }
}

} // namespace

std::u32string text::decode_utf8(const std::string& value)
{
    std::u32string result;
    std::size_t    offset = 0;
    while (offset < value.size())
    {
        result.push_back(read_character(value, offset));
    }

    return result;
}

std::string text::encode_utf8(const std::u32string& value)
{
    std::string result;
    result.reserve(value.size());
    for (const auto character : value)
    {
        if (!is_scalar(character))
        {
            throw std::invalid_argument("Invalid Unicode scalar value");
        }

        const Encoding* encoding = &encodings.front();
        for (const auto& candidate : encodings)
        {
            if (character >= candidate.minimum)
            {
                encoding = &candidate;
            }
        }
        append_character(result, character, *encoding);
    }

    return result;
}

bool text::is_letter(char32_t character) noexcept
{
    return (character >= U'A' && character <= U'Z') || (character >= U'a' && character <= U'z') ||
           (character >= U'А' && character <= U'я') || character == U'Ё' || character == U'ё';
}

bool text::is_word_character(char32_t character) noexcept
{
    return is_letter(character) || (character >= U'0' && character <= U'9') || character == U'_';
}

char32_t text::uppercase(char32_t character) noexcept
{
    if (character >= U'a' && character <= U'z')
    {
        return character - (U'a' - U'A');
    }
    if (character >= U'а' && character <= U'я')
    {
        return character - (U'а' - U'А');
    }
    if (character == U'ё')
    {
        return U'Ё';
    }

    return character;
}
