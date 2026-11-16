#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include <string>


class TextDocument {
public:
    TextDocument() = default;
    explicit TextDocument(std::string text);

    std::string set_text(const std::string& text);

    [[nodiscard]] const std::string& text() const;

    std::string replace_word(const std::string& word, const std::string& replacement);

    [[nodiscard]] std::string find_phrase(const std::string& phrase) const;

    std::string capitalize_words();

    std::string capitalize_sentences();

    std::string number_sentences();

private:
    std::string text_;
};

#endif  
