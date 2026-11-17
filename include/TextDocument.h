#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include <cstddef>
#include <string>

class TextDocument {
public:
    explicit TextDocument(std::string text);

    std::string replaceWords(
        const std::string& oldWord,
        const std::string& newWord
    );

    std::string searchPhrase(
        const std::string& phrase
    ) const;

    std::string capitalizeWords();

    std::string capitalizeSentences();

    std::string numberSentences();

    const std::string& getText() const;

private:
    std::string text_;

    static bool isWordCharacter(unsigned char character);

    static void capitalizeUtf8Character(
        std::string& text,
        std::size_t position
    );
};

#endif // TEXT_DOCUMENT_H
