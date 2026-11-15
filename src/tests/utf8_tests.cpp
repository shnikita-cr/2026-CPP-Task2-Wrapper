#include "test_support.h"
#include "text/utf8.h"

#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{

using test_support::require;
using test_support::require_throws;

void converts_mixed_utf8_text()
{
    const std::string    encoded = u8"Hello, ёж 🐈!";
    const std::u32string decoded = U"Hello, ёж 🐈!";

    require(text::decode_utf8(encoded) == decoded, "ASCII, Russian letters and emoji must decode");
    require(text::encode_utf8(decoded) == encoded, "Mixed Unicode text must encode as UTF-8");
}

void converts_encoding_boundaries()
{
    struct Sample
    {
        const char* name;
        const char* bytes;
        char32_t    character;
    };

    const std::array samples = {
        Sample{"Last ASCII character", "\x7F", U'\x7F'},
        Sample{"First two-byte character", "\xC2\x80", U'\x80'},
        Sample{"Last two-byte character", "\xDF\xBF", U'\x7FF'},
        Sample{"First three-byte character", "\xE0\xA0\x80", U'\x800'},
        Sample{"Character before surrogates", "\xED\x9F\xBF", U'\xD7FF'},
        Sample{"Character after surrogates", "\xEE\x80\x80", U'\xE000'},
        Sample{"Last three-byte character", "\xEF\xBF\xBF", U'\xFFFF'},
        Sample{"First four-byte character", "\xF0\x90\x80\x80", U'\x10000'},
        Sample{"Last Unicode scalar value", "\xF4\x8F\xBF\xBF", U'\x10FFFF'},
    };

    for (const auto& sample : samples)
    {
        const std::u32string decoded(1, sample.character);
        require(text::decode_utf8(sample.bytes) == decoded, sample.name);
        require(text::encode_utf8(decoded) == sample.bytes, sample.name);
    }
}

void preserves_empty_text_and_embedded_nulls()
{
    constexpr std::size_t length = 3;
    const std::string     encoded("a\0b", length);
    const std::u32string  decoded(U"a\0b", length);

    require(text::decode_utf8("").empty(), "Empty UTF-8 must decode to empty text");
    require(text::encode_utf8(U"").empty(), "Empty Unicode text must encode to empty text");
    require(text::decode_utf8(encoded) == decoded, "Decoding must preserve embedded nulls");
    require(text::encode_utf8(decoded) == encoded, "Encoding must preserve embedded nulls");
}

void rejects_invalid_utf8()
{
    const std::array invalid = {
        "\x80",
        "\xBF",
        "\xC2",
        "\xE0\xA0",
        "\xF0\x90\x80",
        "\xC0\xAF",
        "\xC1\xBF",
        "\xC0\x80",
        "\xE0\x80\xAF",
        "\xF0\x80\x80\xAF",
        "\xED\xA0\x80",
        "\xED\xBF\xBF",
        "\xF4\x90\x80\x80",
        "\xF5\x80\x80\x80",
        "\xF8\x88\x80\x80\x80",
        "\xFE",
        "\xFF",
        "\xC2x",
        "\xE2\x28\xA1",
        "a\x80",
        "\xC2\x80\x80",
    };

    for (const auto* encoded : invalid)
    {
        require_throws<std::invalid_argument>(
            [encoded]
            {
                (void)text::decode_utf8(encoded);
            });
    }
}

void rejects_non_scalar_encoding()
{
    constexpr char32_t first_surrogate = 0xD800;
    constexpr char32_t last_surrogate  = 0xDFFF;
    constexpr char32_t beyond_unicode  = 0x110000;
    const std::array   invalid         = {
        first_surrogate,
        last_surrogate,
        beyond_unicode,
        std::numeric_limits<char32_t>::max(),
    };

    for (const auto character : invalid)
    {
        require_throws<std::invalid_argument>(
            [character]
            {
                (void)text::encode_utf8(std::u32string(1, character));
            });
    }
}

void recognizes_letters_and_word_characters()
{
    for (const auto character : std::u32string(U"AZazАЯаяЁё"))
    {
        require(text::is_letter(character), "ASCII and Russian letters must be recognized");
        require(text::is_word_character(character), "Letters must belong to words");
    }
    for (const auto character : std::u32string(U"09_"))
    {
        require(!text::is_letter(character), "Digits and underscore must not be letters");
        require(text::is_word_character(character), "Digits and underscore must belong to words");
    }
    for (const auto character : std::u32string(U" -'!🐈"))
    {
        require(!text::is_letter(character), "Punctuation and emoji must not be letters");
        require(!text::is_word_character(character), "Punctuation and emoji must delimit words");
    }
}

void uppercases_ascii_and_russian_letters()
{
    const std::u32string original = U"azаяёAZАЯЁ09_🐈";
    const std::u32string expected = U"AZАЯЁAZАЯЁ09_🐈";
    std::u32string       converted(original.size(), U'\0');
    std::transform(original.begin(), original.end(), converted.begin(), text::uppercase);

    require(converted == expected, "Case mapping must preserve other valid Unicode characters");
}

} // namespace

int main()
{
    try
    {
        return test_support::run({
            {"converts_mixed_utf8_text", converts_mixed_utf8_text},
            {"converts_encoding_boundaries", converts_encoding_boundaries},
            {"preserves_empty_text_and_embedded_nulls", preserves_empty_text_and_embedded_nulls},
            {"rejects_invalid_utf8", rejects_invalid_utf8},
            {"rejects_non_scalar_encoding", rejects_non_scalar_encoding},
            {"recognizes_letters_and_word_characters", recognizes_letters_and_word_characters},
            {"uppercases_ascii_and_russian_letters", uppercases_ascii_and_russian_letters},
        });
    }
    catch (...)
    {
        return 1;
    }
}
