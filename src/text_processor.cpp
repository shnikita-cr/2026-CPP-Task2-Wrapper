#include "text_processor.h"

#include <cctype>
#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr unsigned char kAsciiLimit = 0x80;
constexpr unsigned char kLeadSmallA = 0xD0;   // а-п, А-Я, Ё
constexpr unsigned char kLeadSmallR = 0xD1;   // р-я, ё
constexpr unsigned char kLowerAFirst = 0xB0;  // 'а' (второй байт после D0)
constexpr unsigned char kLowerPLast = 0xBF;   // 'п'
constexpr unsigned char kCaseShift = 0x20;
constexpr unsigned char kLowerRFirst = 0x80;  // 'р' (второй байт после D1)
constexpr unsigned char kLowerYaLast = 0x8F;  // 'я'
constexpr unsigned char kYoSecond = 0x91;     // 'ё'
constexpr unsigned char kYoUpper = 0x81;      // 'Ё'
constexpr std::string_view kWhitespace = " \t\r\n";
constexpr std::string_view kTerminators = ".!?";

bool isWordByte(char symbol) {
    const auto byte = static_cast<unsigned char>(symbol);
    return std::isalnum(byte) != 0 || byte >= kAsciiLimit;
}

bool isTerminator(char symbol) {
    return kTerminators.find(symbol) != std::string_view::npos;
}

// Делает заглавной букву, начинающуюся в позиции pos (ASCII или кириллица UTF-8).
void capitalizeAt(std::string& text, std::size_t pos) {
    const auto lead = static_cast<unsigned char>(text[pos]);
    if (lead < kAsciiLimit) {
        text[pos] = static_cast<char>(std::toupper(lead));
        return;
    }
    if (pos + 1 >= text.size()) {
        return;
    }
    const auto second = static_cast<unsigned char>(text[pos + 1]);
    if (lead == kLeadSmallA && second >= kLowerAFirst && second <= kLowerPLast) {
        text[pos + 1] = static_cast<char>(second - kCaseShift);
    } else if (lead == kLeadSmallR && second >= kLowerRFirst && second <= kLowerYaLast) {
        text[pos] = static_cast<char>(kLeadSmallA);
        text[pos + 1] = static_cast<char>(second + kCaseShift);
    } else if (lead == kLeadSmallR && second == kYoSecond) {
        text[pos] = static_cast<char>(kLeadSmallA);
        text[pos + 1] = static_cast<char>(kYoUpper);
    }
}

void pushTrimmed(std::vector<std::string>& sentences, const std::string& raw) {
    const auto first = raw.find_first_not_of(kWhitespace);
    if (first == std::string::npos) {
        return;
    }
    const auto last = raw.find_last_not_of(kWhitespace);
    sentences.push_back(raw.substr(first, last - first + 1));
}

std::vector<std::string> splitSentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string current;
    for (std::size_t idx = 0; idx < text.size(); ++idx) {
        current += text[idx];
        const bool lastTerminator = idx + 1 == text.size() || !isTerminator(text[idx + 1]);
        if (isTerminator(text[idx]) && lastTerminator) {
            pushTrimmed(sentences, current);
            current.clear();
        }
    }
    pushTrimmed(sentences, current);
    return sentences;
}

}  // namespace

TextProcessor::TextProcessor(std::string text) : text_(std::move(text)) {}

std::string TextProcessor::replaceWord(const std::string& oldWord, const std::string& newWord) {  // NOLINT(bugprone-easily-swappable-parameters)
    std::string result;
    std::size_t pos = 0;
    while (pos < text_.size()) {
        if (!isWordByte(text_[pos])) {
            result += text_[pos];
            ++pos;
            continue;
        }
        std::size_t end = pos;
        while (end < text_.size() && isWordByte(text_[end])) {
            ++end;
        }
        const std::string word = text_.substr(pos, end - pos);
        result += (word == oldWord) ? newWord : word;
        pos = end;
    }
    text_ = std::move(result);
    return text_;
}

std::string TextProcessor::findPhrase(const std::string& phrase) const {
    return text_.find(phrase) != std::string::npos ? "found" : "not found";
}

std::string TextProcessor::capitalizeWords() {
    bool inWord = false;
    for (std::size_t pos = 0; pos < text_.size(); ++pos) {
        const bool wordByte = isWordByte(text_[pos]);
        if (wordByte && !inWord) {
            capitalizeAt(text_, pos);
        }
        inWord = wordByte;
    }
    return text_;
}

std::string TextProcessor::capitalizeSentences() {
    bool sentenceStart = true;
    for (std::size_t pos = 0; pos < text_.size(); ++pos) {
        const char symbol = text_[pos];
        if (std::isspace(static_cast<unsigned char>(symbol)) != 0) {
            continue;
        }
        if (sentenceStart) {
            capitalizeAt(text_, pos);
        }
        sentenceStart = isTerminator(symbol);
    }
    return text_;
}

std::string TextProcessor::numberSentences() {
    std::string result;
    std::size_t number = 1;
    for (const auto& sentence : splitSentences(text_)) {
        if (number > 1) {
            result += '\n';
        }
        result += std::to_string(number) + ". " + sentence;
        ++number;
    }
    text_ = std::move(result);
    return text_;
}
