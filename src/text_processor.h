#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <cctype>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "wrapper.h"

namespace text_detail {

struct CodePoint {
    unsigned int value;
    std::size_t length;
};

inline CodePoint decode(const std::string& text, std::size_t offset) {
    const auto first = static_cast<unsigned char>(text[offset]);
    if (first < 0x80U) {
        return {first, 1};
    }
    if ((first & 0xE0U) == 0xC0U && offset + 1 < text.size()) {
        const auto second = static_cast<unsigned char>(text[offset + 1]);
        return {((first & 0x1FU) << 6U) | (second & 0x3FU), 2};
    }
    if ((first & 0xF0U) == 0xE0U && offset + 2 < text.size()) {
        const auto second = static_cast<unsigned char>(text[offset + 1]);
        const auto third = static_cast<unsigned char>(text[offset + 2]);
        return {((first & 0x0FU) << 12U) | ((second & 0x3FU) << 6U) |
                    (third & 0x3FU),
                3};
    }
    if ((first & 0xF8U) == 0xF0U && offset + 3 < text.size()) {
        const auto second = static_cast<unsigned char>(text[offset + 1]);
        const auto third = static_cast<unsigned char>(text[offset + 2]);
        const auto fourth = static_cast<unsigned char>(text[offset + 3]);
        return {((first & 0x07U) << 18U) | ((second & 0x3FU) << 12U) |
                    ((third & 0x3FU) << 6U) | (fourth & 0x3FU),
                4};
    }
    return {first, 1};
}

inline bool isLetter(unsigned int value) {
    return (value >= 'a' && value <= 'z') ||
           (value >= 'A' && value <= 'Z') ||
           (value >= 0x0400U && value <= 0x052FU);
}

inline bool isWordCharacter(unsigned int value) {
    return isLetter(value) || (value >= '0' && value <= '9') || value == '_';
}

inline bool isWhitespace(unsigned char value) {
    return std::isspace(value) != 0;
}

inline bool isSentenceMark(unsigned int value) {
    return value == '.' || value == '!' || value == '?';
}

inline unsigned int uppercase(unsigned int value) {
    if (value >= 'a' && value <= 'z') {
        return value - ('a' - 'A');
    }
    if (value >= 0x0430U && value <= 0x044FU) {
        return value - 0x20U;
    }
    if (value >= 0x0450U && value <= 0x045FU) {
        return value - 0x50U;
    }
    return value;
}

inline void appendCodePoint(std::string& output, unsigned int value) {
    if (value < 0x80U) {
        output.push_back(static_cast<char>(value));
    } else if (value < 0x800U) {
        output.push_back(static_cast<char>(0xC0U | (value >> 6U)));
        output.push_back(static_cast<char>(0x80U | (value & 0x3FU)));
    } else if (value < 0x10000U) {
        output.push_back(static_cast<char>(0xE0U | (value >> 12U)));
        output.push_back(static_cast<char>(0x80U | ((value >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (value & 0x3FU)));
    } else {
        output.push_back(static_cast<char>(0xF0U | (value >> 18U)));
        output.push_back(static_cast<char>(0x80U | ((value >> 12U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | ((value >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (value & 0x3FU)));
    }
}

inline bool isWordCharacterBefore(const std::string& text, std::size_t offset) {
    if (offset == 0) {
        return false;
    }
    std::size_t previous = offset - 1;
    while (previous > 0 &&
           (static_cast<unsigned char>(text[previous]) & 0xC0U) == 0x80U) {
        --previous;
    }
    return isWordCharacter(decode(text, previous).value);
}

inline bool isWordCharacterAfter(const std::string& text, std::size_t offset) {
    return offset < text.size() && isWordCharacter(decode(text, offset).value);
}

inline std::string replaceWord(const std::string& text,
                               const std::string& word,
                               const std::string& replacement) {
    if (word.empty()) {
        return text;
    }
    std::string result;
    std::size_t cursor = 0;
    while (cursor < text.size()) {
        const std::size_t match = text.find(word, cursor);
        if (match == std::string::npos) {
            result.append(text, cursor);
            break;
        }
        result.append(text, cursor, match - cursor);
        const std::size_t after = match + word.size();
        if (!isWordCharacterBefore(text, match) &&
            !isWordCharacterAfter(text, after)) {
            result += replacement;
        } else {
            result.append(text, match, word.size());
        }
        cursor = after;
    }
    return result;
}

inline std::string capitalizeWords(const std::string& text) {
    std::string result;
    bool atWordStart = true;
    for (std::size_t cursor = 0; cursor < text.size();) {
        const CodePoint point = decode(text, cursor);
        if (isWordCharacter(point.value)) {
            appendCodePoint(result, atWordStart ? uppercase(point.value) : point.value);
            atWordStart = false;
        } else {
            result.append(text, cursor, point.length);
            atWordStart = true;
        }
        cursor += point.length;
    }
    return result;
}

inline std::string capitalizeSentences(const std::string& text) {
    std::string result;
    bool waitingForLetter = true;
    for (std::size_t cursor = 0; cursor < text.size();) {
        const CodePoint point = decode(text, cursor);
        if (waitingForLetter && isLetter(point.value)) {
            appendCodePoint(result, uppercase(point.value));
            waitingForLetter = false;
        } else {
            result.append(text, cursor, point.length);
            if (isSentenceMark(point.value)) {
                waitingForLetter = true;
            }
        }
        cursor += point.length;
    }
    return result;
}

inline std::vector<std::string> splitSentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::size_t start = 0;
    std::size_t cursor = 0;
    while (cursor < text.size()) {
        const CodePoint point = decode(text, cursor);
        const std::size_t next = cursor + point.length;
        if (isSentenceMark(point.value)) {
            std::size_t end = next;
            while (end < text.size() &&
                   isSentenceMark(decode(text, end).value)) {
                end += decode(text, end).length;
            }
            if (end == text.size() ||
                isWhitespace(static_cast<unsigned char>(text[end]))) {
                std::size_t contentStart = start;
                while (contentStart < cursor &&
                       isWhitespace(static_cast<unsigned char>(text[contentStart]))) {
                    ++contentStart;
                }
                if (contentStart < end) {
                    sentences.emplace_back(text, contentStart, end - contentStart);
                }
                cursor = end;
                while (cursor < text.size() &&
                       isWhitespace(static_cast<unsigned char>(text[cursor]))) {
                    ++cursor;
                }
                start = cursor;
                continue;
            }
        }
        cursor = next;
    }
    std::size_t contentStart = start;
    while (contentStart < text.size() &&
           isWhitespace(static_cast<unsigned char>(text[contentStart]))) {
        ++contentStart;
    }
    if (contentStart < text.size()) {
        std::size_t end = text.size();
        while (end > contentStart &&
               isWhitespace(static_cast<unsigned char>(text[end - 1]))) {
            --end;
        }
        sentences.emplace_back(text, contentStart, end - contentStart);
    }

    return sentences;
}

inline std::string numberSentences(const std::string& text) {
    const std::vector<std::string> sentences = splitSentences(text);
    std::string result;
    for (std::size_t index = 0; index < sentences.size(); ++index) {
        if (index != 0) {
            result.push_back('\n');
        }
        result += std::to_string(index + 1);
        result += ". ";
        result += sentences[index];
    }
    return result;
}

}  // namespace text_detail

class TextProcessor {
public:
    std::string replaceWords(const Arguments& arguments) const {
        return text_detail::replaceWord(arguments.at("text"),
                                        arguments.at("word"),
                                        arguments.at("replacement"));
    }

    std::string findPhrase(const std::string& text,
                           const std::string& phrase) const {
        return text.find(phrase) != std::string::npos
                   ? "Найдено"
                   : "Не найдено";
    }

    std::string capitalizeEachWord(const Arguments& arguments) const {
        return text_detail::capitalizeWords(arguments.at("text"));
    }

    std::string capitalizeSentenceStarts(const Arguments& arguments) const {
        return text_detail::capitalizeSentences(arguments.at("text"));
    }

    std::string numberEachSentence(const Arguments& arguments) const {
        return text_detail::numberSentences(arguments.at("text"));
    }
};

#endif  // TEXT_PROCESSOR_H
