#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

class TextProcessor {
public:
    [[nodiscard]] std::string replace_words(std::string_view text,
                                            std::string_view from,
                                            const std::string& target);

    [[nodiscard]] bool find_phrase(std::string_view text,
                                   std::string_view phrase);

    [[nodiscard]] std::string capitalize_words(std::string_view text);

    [[nodiscard]] std::string capitalize_sentences(std::string_view text);

    [[nodiscard]] std::string number_sentences(std::string_view text);

    [[nodiscard]] std::size_t operations_count() const noexcept;

private:
    [[nodiscard]] static std::vector<std::string> split_sentences(std::string_view text);

    std::size_t operations_count_ = 0;
};