#pragma once
#include <string>

class TextProcessor {
public:
    explicit TextProcessor(std::string text);
    [[nodiscard]] const std::string& text() const;
    std::string replace_words(const std::string& word, const std::string& replacement);
    [[nodiscard]] std::string find_phrase(const std::string& phrase) const;
    std::string capitalize_words();
    std::string capitalize_sentences();
    std::string number_sentences();
private:
    std::string text_;
};
