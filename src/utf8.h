#ifndef UTF8_H
#define UTF8_H

#include <string>
#include <string_view>

namespace utf8 {

[[nodiscard]] std::u32string decode(std::string_view bytes);

[[nodiscard]] std::string encode(std::u32string_view text);

[[nodiscard]] char32_t to_upper(char32_t symbol);
[[nodiscard]] char32_t to_lower(char32_t symbol);

[[nodiscard]] bool is_letter(char32_t symbol);
[[nodiscard]] bool is_digit(char32_t symbol);
[[nodiscard]] bool is_space(char32_t symbol);

}

#endif