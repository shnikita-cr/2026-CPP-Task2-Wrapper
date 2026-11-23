#include "TextDocument.h"

#include <utility>
#include <cctype>

auto isWordChar(char character) -> bool {
    return std::isalnum(static_cast<unsigned char>(character)) != 0 || character == '_';
}

auto isSentenceEnd(char character) -> bool {
    return character == '.' || character == '!' || character == '?';
}

auto toUpper(char character) -> char {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
}

TextDocument::TextDocument(std::string text): text_(std::move(text)) {}

auto TextDocument::getText() const -> const std::string& {
    return text_;
}

auto TextDocument::replaceWords(const std::string& oldWord, const std::string& newWord) -> std::string {

    if (oldWord.empty()) {
        return text_;
    }

    std::size_t pos = 0;

    while ((pos = text_.find(oldWord, pos)) != std::string::npos) {

        bool leftBoundary = pos == 0 || !isWordChar(text_[pos - 1]);

        std::size_t end = pos + oldWord.size();

        bool rightBoundary = end == text_.size() || !isWordChar(text_[end]);

        if (leftBoundary && rightBoundary) {
            text_.replace(pos, oldWord.size(), newWord);
            pos += newWord.size();
        }
        else {
            pos += oldWord.size();
        }
    }

    return text_;
}

auto TextDocument::findPhrase(const std::string& phrase) const -> std::string {
    if (phrase.empty()) {
        return "not found";
    }

    if (text_.find(phrase) != std::string::npos) {
        return "found";
    }
    
    return "not found";

}

auto TextDocument::capitalizeWords() -> std::string {
    bool wordStart = true;

    for (char& character : text_) {

        if (isWordChar(character)) {
            if (wordStart) {
                character = toUpper(character);
            }
            wordStart = false;
        }
        else {
            wordStart = true;
        }
    }

    return text_;
}

auto TextDocument::capitalizeSentences() -> std::string {
    bool sentenceStart = true;

    for (char& character : text_) {
        if (sentenceStart && std::isalpha(static_cast<unsigned char>(character)) != 0) {
            character = toUpper(character);
            sentenceStart = false;
        }
        else if (isWordChar(character)) {
            sentenceStart = false;
        }

        if (isSentenceEnd(character)) {
            sentenceStart = true;
        }
    }

    return text_;
}

auto TextDocument::numberSentences() -> std::string {
    std::string result;
    std::size_t start = 0;
    std::size_t number = 1;

    while (start < text_.size()) {
        while (start < text_.size() && std::isspace(static_cast<unsigned char>(text_[start])) != 0) {
            ++start;
        }

        if (start == text_.size()) {
            break;
        }

        std::size_t end = start;

        while (end < text_.size() && !isSentenceEnd(text_[end])) {
            ++end;
        }

        if (end < text_.size()) {
            while (end + 1 < text_.size() && isSentenceEnd(text_[end + 1])) {
                ++end;
            }
            ++end;
        }

        if (!result.empty()) {
            result += '\n';
        }

        result += std::to_string(number++);
        result += ". ";
        result += text_.substr(start, end - start);

        start = end;
    }

    text_ = std::move(result);
    return text_;
}