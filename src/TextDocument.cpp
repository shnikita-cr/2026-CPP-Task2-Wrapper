#include "TextDocument.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <utility>
#include <vector>

namespace {

bool isWordSymbol(char symbol) {
    return std::isalnum(static_cast<unsigned char>(symbol)) != 0 || symbol == '\'';
}

bool isSentenceEnd(char symbol) {
    return symbol == '.' || symbol == '!' || symbol == '?';
}

char toUpperSymbol(char symbol) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(symbol)));
}

std::string capitalizeFirst(std::string word) {
    if (!word.empty()) {
        word.front() = toUpperSymbol(word.front());
    }
    return word;
}

template <typename Handler>
std::string mapWords(const std::string& text, Handler handler) {
    std::string result;
    result.reserve(text.size());
    std::string word;
    for (const char symbol : text) {
        if (isWordSymbol(symbol)) {
            word.push_back(symbol);
        } else {
            result += handler(word);
            word.clear();
            result.push_back(symbol);
        }
    }
    result += handler(word);
    return result;
}

std::string trim(const std::string& text) {
    const auto isSpace = [](char symbol) { return std::isspace(static_cast<unsigned char>(symbol)) != 0; };
    const auto first = std::find_if_not(text.begin(), text.end(), isSpace);
    const auto last = std::find_if_not(text.rbegin(), text.rend(), isSpace).base();
    if (first >= last) {
        return {};
    }
    std::string result(first, last);
    return result;
}

void appendSentence(std::vector<std::string>& sentences, std::string& current) {
    const std::string sentence = trim(current);
    if (!sentence.empty()) {
        sentences.push_back(sentence);
    }
    current.clear();
}

std::vector<std::string> splitSentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string current;
    for (std::size_t index = 0; index < text.size(); ++index) {
        current.push_back(text[index]);
        const bool lastSymbol = index + 1 == text.size();
        if (isSentenceEnd(text[index]) && (lastSymbol || !isSentenceEnd(text[index + 1]))) {
            appendSentence(sentences, current);
        }
    }
    appendSentence(sentences, current);
    return sentences;
}

} // namespace

TextDocument::TextDocument(std::string text) : text_(std::move(text)) {}

const std::string& TextDocument::text() const noexcept {
    return text_;
}

std::string TextDocument::replaceWords(const std::string& oldWord, const std::string& newWord) {
    text_ = mapWords(text_, [&oldWord, &newWord](const std::string& word) { return (!word.empty() && word == oldWord) ? newWord : word; });
    return text_;
}

std::string TextDocument::findPhrase(const std::string& phrase) const {
    const bool found = !phrase.empty() && text_.find(phrase) != std::string::npos;
    return found ? "found" : "not found";
}

std::string TextDocument::capitalizeWords() {
    text_ = mapWords(text_, capitalizeFirst);
    return text_;
}

std::string TextDocument::capitalizeSentences() {
    bool sentenceStart = true;
    for (char& symbol : text_) {
        if (isSentenceEnd(symbol)) {
            sentenceStart = true;
        } else if (isWordSymbol(symbol)) {
            if (sentenceStart) {
                symbol = toUpperSymbol(symbol);
            }
            sentenceStart = false;
        }
    }
    return text_;
}

std::string TextDocument::numberSentences(const std::string& delimiter) {
    std::string result;
    std::size_t number = 1;
    for (const std::string& sentence : splitSentences(text_)) {
        if (!result.empty()) {
            result += '\n';
        }
        result += std::to_string(number);
        result += delimiter;
        result += sentence;
        ++number;
    }
    text_ = std::move(result);
    return text_;
}
