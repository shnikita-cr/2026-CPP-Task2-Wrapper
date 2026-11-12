// Операции меняют текст и возвращают его. Заглавными становятся
// только латинские буквы. Конец предложения: '.', '!' или '?' перед пробелом.
#pragma once

#include <string>

class TextDocument {
public:
    explicit TextDocument(std::string text);

    [[nodiscard]] const std::string& text() const;

    // Заменяет только целые слова.
    std::string replace_word(const std::string& word, const std::string& replacement);

    // Возвращает "found" или "not found".
    [[nodiscard]] std::string find_phrase(const std::string& phrase) const;

    std::string capitalize_words();

    std::string capitalize_sentences();

    std::string number_sentences();

private:
    std::string text_;
};
