#include "text_processor.h"
#include <algorithm>
#include <utility>
#include <stdexcept>

namespace {
bool is_letter(char letter) {
    return (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z');
}
bool is_word(char letter) {
    return is_letter(letter) || (letter >= '0' && letter <= '9') || letter == '_';
}
char uppercase(char letter) {
    if (letter >= 'a' && letter <= 'z') {
        return static_cast<char>(letter - 'a' + 'A');
    }
    return letter;
}
bool is_end(char letter) {
    return letter == '.' || letter == '!' || letter == '?';
}
bool sentence_end(const std::string& text, std::size_t index) {
    if (!is_end(text[index])) {
        return false;
    }
    const auto digit = [](char symbol) { return symbol >= '0' && symbol <= '9'; };
    return !(text[index] == '.' && index > 0 && index + 1 < text.size() &&
             digit(text[index - 1]) && digit(text[index + 1]));
}
bool is_space(char letter) {
    return letter == ' ' || letter == '\n' || letter == '\r' || letter == '\t';
}
bool has_boundaries(const std::string& text, std::size_t start, std::size_t length) {
    return (start == 0 || !is_word(text[start - 1])) &&
           (start + length == text.size() || !is_word(text[start + length]));
}
std::string normalized(const std::string& text) {
    std::string result;
    for (const auto letter : text) {
        if (!is_space(letter)) {
            result += letter;
        } else if (!result.empty() && result.back() != ' ') {
            result += ' ';
        }
    }
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}
}

TextProcessor::TextProcessor(std::string text) : text_(std::move(text)) {}

const std::string& TextProcessor::text() const {
    return text_;
}

std::string TextProcessor::replace_words(const std::string& word,
                                         const std::string& replacement) {
    if (word.empty()) {
        throw std::invalid_argument("Replacement word must not be empty");
    }
    const auto& source = text_;
    const auto& target = word;
    if (!std::all_of(target.begin(), target.end(), is_word)) {
        throw std::invalid_argument("Expected one word");
    }
    if (word == replacement) {
        return text_;
    }
    const auto& substitute = replacement;
    std::string result;
    std::size_t cursor = 0;
    while (cursor < source.size()) {
        if (source.compare(cursor, target.size(), target) == 0 &&
            has_boundaries(source, cursor, target.size())) {
            result += substitute;
            cursor += target.size();
        } else {
            result += source[cursor++];
        }
    }
    text_ = std::move(result);
    return text_;
}

std::string TextProcessor::find_phrase(const std::string& phrase) const {
    const auto target = normalized(phrase);
    if (target.empty()) {
        throw std::invalid_argument("Search phrase must not be empty");
    }
    const auto source = normalized(text_);
    auto position = source.find(target);
    while (position != std::string::npos) {
        if (has_boundaries(source, position, target.size())) {
            return "found";
        }
        position = source.find(target, position + 1);
    }
    return "not found";
}
std::string TextProcessor::capitalize_words() {
    auto result = text_;
    bool start = true;
    for (auto& letter : result) {
        if (start && is_letter(letter)) {
            letter = uppercase(letter);
        }
        start = !is_word(letter) && letter != '\'';
    }
    text_ = std::move(result);
    return text_;
}
std::string TextProcessor::capitalize_sentences() {
    auto result = text_;
    bool start = true;
    for (std::size_t index = 0; index < result.size(); ++index) {
        auto& letter = result[index];
        if (start && is_letter(letter)) {
            letter = uppercase(letter);
            start = false;
        }
        if (sentence_end(result, index)) {
            start = true;
        }
    }
    text_ = std::move(result);
    return text_;
}
std::string TextProcessor::number_sentences() {
    const auto source = normalized(text_);
    std::string result;
    std::size_t cursor = 0;
    std::size_t number = 1;
    while (cursor < source.size()) {
        while (cursor < source.size() && is_space(source[cursor])) {
            ++cursor;
        }
        const auto start = cursor;
        while (cursor < source.size() && !sentence_end(source, cursor)) {
            ++cursor;
        }
        while (cursor < source.size() && is_end(source[cursor])) {
            ++cursor;
        }
        auto end = cursor;
        while (end > start && is_space(source[end - 1])) {
            --end;
        }
        if (end > start) {
            if (!result.empty()) {
                result += '\n';
            }
            result += std::to_string(number++) + ". " + source.substr(start, end - start);
        }
    }
    text_ = std::move(result);
    return text_;
}
