#include "document.hpp"

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

bool is_word_char(unsigned char symbol) {
    return std::isalnum(symbol) != 0 || symbol == '_';
}

std::vector<std::string> split_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string current;
    for (const char symbol : text) {
        if (symbol == '\n' || symbol == '\r') {
            continue;
        }
        current.push_back(symbol);
        if (symbol == '.' || symbol == '!' || symbol == '?') {
            sentences.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        sentences.push_back(current);
    }
    return sentences;
}

std::string trim_spaces(const std::string& text) {
    std::size_t begin = 0;
    while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])) != 0) {
        ++begin;
    }
    std::size_t end = text.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
        --end;
    }
    return text.substr(begin, end - begin);
}

}  // namespace

Document::Document(std::string text) : text_(std::move(text)) {}

bool Document::matches_word_at(std::size_t position, const std::string& word) const {
    if (text_.compare(position, word.size(), word) != 0) {
        return false;
    }
    const bool left_ok = position == 0 || !is_word_char(static_cast<unsigned char>(text_[position - 1]));
    const std::size_t after = position + word.size();
    const bool right_ok = after == text_.size() || !is_word_char(static_cast<unsigned char>(text_[after]));
    return left_ok && right_ok;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string Document::replace_words(const std::string& from, const std::string& replacement) {
    if (from.empty()) {
        throw std::runtime_error("the word to replace must not be empty");
    }
    std::string result;
    result.reserve(text_.size());
    std::size_t position = 0;
    while (position < text_.size()) {
        if (matches_word_at(position, from)) {
            result += replacement;
            position += from.size();
        } else {
            result.push_back(text_[position]);
            ++position;
        }
    }
    text_ = std::move(result);
    return text_;
}

std::string Document::find_phrases(const std::string& phrase) const {
    if (phrase.empty()) {
        throw std::runtime_error("the phrase to search must not be empty");
    }
    return text_.find(phrase) == std::string::npos ? "not found" : "found";
}

std::string Document::capitalize_words() {
    bool at_word_start = true;
    for (char& symbol : text_) {
        const auto value = static_cast<unsigned char>(symbol);
        if (!is_word_char(value)) {
            at_word_start = true;
            continue;
        }
        if (at_word_start && std::isalpha(value) != 0) {
            symbol = static_cast<char>(std::toupper(value));
        }
        at_word_start = false;
    }
    return text_;
}

std::string Document::capitalize_sentences() {
    bool at_sentence_start = true;
    for (char& symbol : text_) {
        const auto value = static_cast<unsigned char>(symbol);
        if (std::isalpha(value) != 0) {
            if (at_sentence_start) {
                symbol = static_cast<char>(std::toupper(value));
            }
            at_sentence_start = false;
        } else if (symbol == '.' || symbol == '!' || symbol == '?') {
            at_sentence_start = true;
        }
    }
    return text_;
}

std::string Document::number_sentences() {
    std::ostringstream stream;
    int number = 1;
    bool first = true;
    for (const std::string& sentence : split_sentences(text_)) {
        const std::string item = trim_spaces(sentence);
        if (item.empty()) {
            continue;
        }
        if (!first) {
            stream << '\n';
        }
        first = false;
        stream << number << ". " << item;
        ++number;
    }
    text_ = stream.str();
    return text_;
}
