#include "TextDocument.h"

#include <cctype>
#include <sstream>
#include <string>
#include <utility>

namespace {
    constexpr unsigned char kAsciiLimit = 128;
    constexpr unsigned char kUtf8FirstByteD0 = 0xD0;
    constexpr unsigned char kUtf8FirstByteD1 = 0xD1;
    constexpr unsigned char kUtf8LowerYo = 0x91;
    constexpr unsigned char kUtf8UpperYo = 0x81;
    constexpr unsigned char kUtf8LowerStart = 0x80;
    constexpr unsigned char kUtf8LowerEnd = 0x8F;
    constexpr unsigned char kUtf8UpperStart = 0xB0;
    constexpr unsigned char kUtf8UpperEnd = 0xBF;
    constexpr unsigned char kUtf8CaseDifference = 0x20;
}

TextDocument::TextDocument(std::string text)
    : text_(std::move(text)) {
}

bool TextDocument::isWordCharacter(unsigned char character) {
    return std::isalnum(character) != 0
        || character >= kAsciiLimit
        || character == '_';
}

void TextDocument::capitalizeUtf8Character(
    std::string& text,
    std::size_t position
) {
    if (position >= text.size()) {
        return;
    }

    const auto first =
        static_cast<unsigned char>(text[position]);

    if (first == kUtf8FirstByteD1
        && position + 1 < text.size()) {

        const auto second =
            static_cast<unsigned char>(text[position + 1]);

        if (second == kUtf8LowerYo) {
            text[position] = static_cast<char>(kUtf8FirstByteD0);
            text[position + 1] = static_cast<char>(kUtf8UpperYo);
            return;
        }

        if (second >= kUtf8LowerStart
            && second <= kUtf8LowerEnd) {

            text[position] = static_cast<char>(kUtf8FirstByteD0);
            text[position + 1] = static_cast<char>(
                second - kUtf8CaseDifference
                );
            return;
        }
    }

    if (first == kUtf8FirstByteD0
        && position + 1 < text.size()) {

        const auto second =
            static_cast<unsigned char>(text[position + 1]);

        if (second >= kUtf8UpperStart
            && second <= kUtf8UpperEnd) {

            text[position + 1] = static_cast<char>(
                second - kUtf8CaseDifference
                );
        }
    }

    if (first < kAsciiLimit) {
        text[position] = static_cast<char>(
            std::toupper(first)
            );
    }
}

std::string TextDocument::replaceWords(
    const std::string& oldWord,
    const std::string& newWord
) {
    if (oldWord.empty()) {
        return text_;
    }

    std::size_t position = 0;

    while ((position = text_.find(oldWord, position))
        != std::string::npos) {

        const bool leftBoundary =
            position == 0
            || !isWordCharacter(
                static_cast<unsigned char>(text_[position - 1])
            );

        const std::size_t end = position + oldWord.size();

        const bool rightBoundary =
            end >= text_.size()
            || !isWordCharacter(
                static_cast<unsigned char>(text_[end])
            );

        if (leftBoundary && rightBoundary) {
            text_.replace(position, oldWord.size(), newWord);
            position += newWord.size();
        }
        else {
            position += oldWord.size();
        }
    }

    return text_;
}

std::string TextDocument::searchPhrase(
    const std::string& phrase
) const {
    if (phrase.empty()) {
        return "Empty search phrase";
    }

    std::ostringstream result;
    std::size_t position = 0;
    bool found = false;

    while ((position = text_.find(phrase, position))
        != std::string::npos) {

        if (found) {
            result << ", ";
        }

        result << position;
        found = true;
        position += phrase.size();
    }

    if (!found) {
        return "Phrase not found";
    }

    return "Found at positions: " + result.str();
}

std::string TextDocument::capitalizeWords() {
    bool newWord = true;

    for (std::size_t i = 0; i < text_.size(); ++i) {
        const auto character =
            static_cast<unsigned char>(text_[i]);

        if (isWordCharacter(character)) {
            if (newWord) {
                capitalizeUtf8Character(text_, i);
            }

            newWord = false;
        }
        else {
            newWord = true;
        }
    }

    return text_;
}

std::string TextDocument::capitalizeSentences() {
    bool sentenceStart = true;

    for (std::size_t i = 0; i < text_.size(); ++i) {
        const auto character =
            static_cast<unsigned char>(text_[i]);

        if (sentenceStart && isWordCharacter(character)) {
            capitalizeUtf8Character(text_, i);
            sentenceStart = false;
        }

        if (character == '.' || character == '!'
            || character == '?') {
            sentenceStart = true;
        }
    }

    return text_;
}

std::string TextDocument::numberSentences() {
    std::ostringstream result;
    std::string sentence;
    std::size_t sentenceNumber = 1;

    for (std::size_t i = 0; i < text_.size(); ++i) {
        const char character = text_[i];
        sentence += character;

        const bool isEndMark =
            character == '.' || character == '!' || character == '?';

        const bool nextIsEndMark =
            i + 1 < text_.size()
            && (text_[i + 1] == '.'
                || text_[i + 1] == '!'
                || text_[i + 1] == '?');

        if (isEndMark && !nextIsEndMark) {
            std::size_t start = 0;

            while (start < sentence.size()
                && std::isspace(
                    static_cast<unsigned char>(sentence[start])
                ) != 0) {
                ++start;
            }

            if (start < sentence.size()) {
                if (sentenceNumber > 1) {
                    result << '\n';
                }

                result << sentenceNumber << ". "
                    << sentence.substr(start);

                ++sentenceNumber;
            }

            sentence.clear();
        }
    }

    std::size_t start = 0;

    while (start < sentence.size()
        && std::isspace(
            static_cast<unsigned char>(sentence[start])
        ) != 0) {
        ++start;
    }

    if (start < sentence.size()) {
        if (sentenceNumber > 1) {
            result << '\n';
        }

        result << sentenceNumber << ". "
            << sentence.substr(start);
    }

    return result.str();
}

const std::string& TextDocument::getText() const {
    return text_;
}
