#include "TextDocument.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

constexpr unsigned char kFirstNonAsciiByte = 0x80;

bool is_word_char(char symbol) {
    const auto byte = static_cast<unsigned char>(symbol);
    return std::isalnum(byte) != 0 || symbol == '_' || byte >= kFirstNonAsciiByte;
}

bool is_space(char symbol) {
    return std::isspace(static_cast<unsigned char>(symbol)) != 0;
}

char to_upper(char symbol) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(symbol)));
}

bool is_terminator(char symbol) {
    return symbol == '.' || symbol == '!' || symbol == '?';
}

// Апостроф внутри слова ("don't") новое слово не начинает.
bool starts_word(const std::string& text, std::size_t pos) {
    if (!is_word_char(text[pos])) {
        return false;
    }
    return pos == 0 || (!is_word_char(text[pos - 1]) && text[pos - 1] != '\'');
}

bool ends_sentence(const std::string& text, std::size_t pos) {
    const std::size_t next = pos + 1;
    return is_terminator(text[pos]) && (next == text.size() || is_space(text[next]));
}

std::string collapse_spaces(const std::string& text) {
    std::string result;
    bool pending_space = false;
    for (const char symbol : text) {
        if (is_space(symbol)) {
            pending_space = !result.empty();
            continue;
        }
        if (pending_space) {
            result += ' ';
            pending_space = false;
        }
        result += symbol;
    }
    return result;
}

std::vector<std::string> split_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::size_t begin = 0;
    for (std::size_t pos = 0; pos < text.size(); ++pos) {
        if (ends_sentence(text, pos)) {
            sentences.push_back(collapse_spaces(text.substr(begin, pos + 1 - begin)));
            begin = pos + 1;
        }
    }
    sentences.push_back(collapse_spaces(text.substr(begin)));
    if (sentences.back().empty()) {
        sentences.pop_back();
    }
    return sentences;
}

bool is_single_word(const std::string& text) {
    return !text.empty() && std::all_of(text.begin(), text.end(), is_word_char);
}

}  // namespace

TextDocument::TextDocument(std::string text) : text_(std::move(text)) {}

const std::string& TextDocument::text() const {
    return text_;
}

std::string TextDocument::replace_word(const std::string& word, const std::string& replacement) {
    if (!is_single_word(word)) {
        throw std::invalid_argument("'" + word + "' is not a single word");
    }
    std::string result;
    std::size_t pos = 0;
    while (pos < text_.size()) {
        if (!is_word_char(text_[pos])) {
            result += text_[pos];
            ++pos;
            continue;
        }
        std::size_t end = pos;
        while (end < text_.size() && is_word_char(text_[end])) {
            ++end;
        }
        const std::string current = text_.substr(pos, end - pos);
        result += current == word ? replacement : current;
        pos = end;
    }
    text_ = std::move(result);
    return text_;
}

std::string TextDocument::find_phrase(const std::string& phrase) const {
    const std::string target = collapse_spaces(phrase);
    if (target.empty()) {
        throw std::invalid_argument("phrase must not be empty");
    }
    const std::string text = collapse_spaces(text_);
    for (std::size_t pos = text.find(target); pos != std::string::npos; pos = text.find(target, pos + 1)) {
        const std::size_t end = pos + target.size();
        const bool starts_on_boundary = pos == 0 || !is_word_char(text[pos - 1]);
        const bool ends_on_boundary = end == text.size() || !is_word_char(text[end]);
        if (starts_on_boundary && ends_on_boundary) {
            return "found";
        }
    }
    return "not found";
}

std::string TextDocument::capitalize_words() {
    for (std::size_t pos = 0; pos < text_.size(); ++pos) {
        if (starts_word(text_, pos)) {
            text_[pos] = to_upper(text_[pos]);
        }
    }
    return text_;
}

std::string TextDocument::capitalize_sentences() {
    bool sentence_start = true;
    for (std::size_t pos = 0; pos < text_.size(); ++pos) {
        if (sentence_start && is_word_char(text_[pos])) {
            text_[pos] = to_upper(text_[pos]);
            sentence_start = false;
        } else if (ends_sentence(text_, pos)) {
            sentence_start = true;
        }
    }
    return text_;
}

std::string TextDocument::number_sentences() {
    std::string result;
    std::size_t number = 1;
    for (const std::string& sentence : split_sentences(text_)) {
        if (!result.empty()) {
            result += '\n';
        }
        result += std::to_string(number) + ". " + sentence;
        ++number;
    }
    text_ = std::move(result);
    return text_;
}
