#include "text_document.h"

#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {


struct Range {
    std::size_t begin;
    std::size_t end;
};


bool is_alnum(char symbol) {
    return std::isalnum(static_cast<unsigned char>(symbol)) != 0;
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

bool is_closing(char symbol) {
    return symbol == '"' || symbol == '\'' || symbol == ')' || symbol == ']';
}


bool is_word_char(const std::string& text, std::size_t index) {
    if (is_alnum(text[index])) {
        return true;
    }
    if (text[index] != '\'' || index == 0 || index + 1 >= text.size()) {
        return false;
    }
    return is_alnum(text[index - 1]) && is_alnum(text[index + 1]);
}

bool starts_word(const std::string& text, std::size_t index) {
    return index == 0 || !is_word_char(text, index - 1);
}

bool ends_word(const std::string& text, std::size_t index) {
    return index >= text.size() || !is_word_char(text, index);
}


bool matches_at(const std::string& text, std::size_t position, const std::string& pattern) {
    if (text.compare(position, pattern.size(), pattern) != 0) {
        return false;
    }
    const bool left_ok = !is_alnum(pattern.front()) || starts_word(text, position);
    const bool right_ok =
        !is_alnum(pattern.back()) || ends_word(text, position + pattern.size());
    return left_ok && right_ok;
}

void require_not_empty(const std::string& pattern, const char* what) {
    if (pattern.empty()) {
        throw std::invalid_argument(std::string("TextDocument: ") + what +
                                    " must not be empty");
    }
}

std::size_t skip_while(const std::string& text, std::size_t position, bool (*predicate)(char)) {
    while (position < text.size() && predicate(text[position])) {
        ++position;
    }
    return position;
}


std::vector<Range> split_sentences(const std::string& text) {
    std::vector<Range> sentences;
    std::size_t start = 0;
    std::size_t position = 0;
    while (position < text.size()) {
        if (!is_terminator(text[position])) {
            ++position;
            continue;
        }
        position = skip_while(text, position, is_terminator);
        position = skip_while(text, position, is_closing);
        if (position == text.size() || is_space(text[position])) {
            sentences.push_back({start, position});
            start = position;
        }
    }
    if (start < text.size()) {
        sentences.push_back({start, text.size()});
    }
    return sentences;
}


std::string normalize_spaces(const std::string& text, Range range) {
    std::string result;
    bool pending_space = false;
    for (std::size_t index = range.begin; index < range.end; ++index) {
        const char symbol = text[index];
        if (is_space(symbol)) {
            pending_space = !result.empty();
            continue;
        }
        if (pending_space) {
            result.push_back(' ');
            pending_space = false;
        }
        result.push_back(symbol);
    }
    return result;
}

} 

TextDocument::TextDocument(std::string text) : text_(std::move(text)) {}

std::string TextDocument::set_text(const std::string& text) {
    text_ = text;
    return text_;
}

const std::string& TextDocument::text() const {
    return text_;
}

std::string TextDocument::replace_word(const std::string& word, const std::string& replacement) {
    require_not_empty(word, "word to replace");
    if (word == replacement) {
        return text_;
    }

    std::string result;
    result.reserve(text_.size());
    std::size_t position = 0;
    while (position < text_.size()) {
        if (matches_at(text_, position, word)) {
            result += replacement;
            position += word.size();
        } else {
            result.push_back(text_[position]);
            ++position;
        }
    }
    text_ = std::move(result);
    return text_;
}

std::string TextDocument::find_phrase(const std::string& phrase) const {
    require_not_empty(phrase, "phrase to find");

    std::size_t position = text_.find(phrase);
    while (position != std::string::npos) {
        if (matches_at(text_, position, phrase)) {
            return "found";
        }
        position = text_.find(phrase, position + 1);
    }
    return "not found";
}

std::string TextDocument::capitalize_words() {
    for (std::size_t index = 0; index < text_.size(); ++index) {
        if (is_alnum(text_[index]) && starts_word(text_, index)) {
            text_[index] = to_upper(text_[index]);
        }
    }
    return text_;
}

std::string TextDocument::capitalize_sentences() {
    for (const Range& sentence : split_sentences(text_)) {
        for (std::size_t index = sentence.begin; index < sentence.end; ++index) {
            if (is_alnum(text_[index])) {
                text_[index] = to_upper(text_[index]);
                break;
            }
        }
    }
    return text_;
}

std::string TextDocument::number_sentences() {
    std::string result;
    std::size_t number = 0;
    for (const Range& sentence : split_sentences(text_)) {
        const std::string content = normalize_spaces(text_, sentence);
        if (content.empty()) {
            continue;
        }
        if (number > 0) {
            result.push_back('\n');
        }
        ++number;
        result += std::to_string(number) + ". " + content;
    }
    text_ = std::move(result);
    return text_;
}
