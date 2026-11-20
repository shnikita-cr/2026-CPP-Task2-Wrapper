#pragma once

#include <string>
#include <string_view>

class TextProcessor {
public:
    explicit TextProcessor(std::string text); // Prevent converting constructor behavior

    std::string replace_words(const std::string& old_word, const std::string& new_word);
    [[nodiscard]] std::string_view find_phrase(const std::string& phrase) const;
    std::string capitalize_each_word();
    std::string capitalize_sentence_starts();
    std::string number_sentences();
    [[nodiscard]] const std::string& text() const;
private:
    std::string my_text;
};