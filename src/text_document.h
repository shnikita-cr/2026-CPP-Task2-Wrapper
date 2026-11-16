#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include <string>

class TextDocument {
public:
    explicit TextDocument(std::string text);

    [[nodiscard]] const std::string& text() const;

    const std::string& replace_words(const std::string& replaced, const std::string& replacement);

    [[nodiscard]] bool find_phrase(const std::string& phrase) const;

    const std::string& capitalize_words();

    const std::string& capitalize_sentences();

    const std::string& number_sentences();

private:
    std::string m_text;
};

#endif