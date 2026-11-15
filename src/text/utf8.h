#ifndef WRAPPER_TEXT_UTF8_H
#define WRAPPER_TEXT_UTF8_H

#include <string>

namespace text
{

[[nodiscard]] std::u32string decode_utf8(const std::string& value);
[[nodiscard]] std::string    encode_utf8(const std::u32string& value);
[[nodiscard]] bool           is_letter(char32_t character) noexcept;
[[nodiscard]] bool           is_word_character(char32_t character) noexcept;
[[nodiscard]] char32_t       uppercase(char32_t character) noexcept;

} // namespace text

#endif // WRAPPER_TEXT_UTF8_H
