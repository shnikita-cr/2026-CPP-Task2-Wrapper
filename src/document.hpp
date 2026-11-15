#pragma once

#include <cstddef>
#include <string>

// Текстовый документ, операции над которым выполняются через Wrapper.
class Document {
public:
    explicit Document(std::string text);

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    std::string replace_words(const std::string& from, const std::string& replacement);
    [[nodiscard]] std::string find_phrases(const std::string& phrase) const;
    std::string capitalize_words();
    std::string capitalize_sentences();
    std::string number_sentences();

private:
    std::string text_;

    [[nodiscard]] bool matches_word_at(std::size_t position, const std::string& word) const;
};
