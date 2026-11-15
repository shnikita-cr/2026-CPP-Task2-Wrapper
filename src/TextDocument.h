#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include <string>

class TextDocument {
public:
    explicit TextDocument(std::string text);

    [[nodiscard]] const std::string& text() const noexcept;

    std::string replaceWords(const std::string& oldWord, const std::string& newWord);

    [[nodiscard]] std::string findPhrase(const std::string& phrase) const;

    std::string capitalizeWords();

    std::string capitalizeSentences();

    std::string numberSentences(const std::string& delimiter);

private:
    std::string text_;
};

#endif
