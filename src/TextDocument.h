#pragma once

#include <string>

class TextDocument {
private:
    std::string text_;

public:
    explicit TextDocument(std::string text);

    std::string replaceWords(const std::string& oldWord,const std::string& newWord);

    std::string findPhrase(const std::string& phrase) const;

    std::string capitalizeWords();

    std::string capitalizeSentences();

    std::string numberSentences();

    const std::string& getText() const;
};