#include "TextEditor.hpp"

#include <cctype>
#include <cstddef>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

namespace {

bool is_word_char(char symbol) {
    return std::isalnum(static_cast<unsigned char>(symbol)) != 0;
}

bool is_letter(char symbol) {
    return std::isalpha(static_cast<unsigned char>(symbol)) != 0;
}

bool is_space(char symbol) {
    return std::isspace(static_cast<unsigned char>(symbol)) != 0;
}

bool is_sentence_terminator(char symbol) {
    return symbol == '.' || symbol == '!' || symbol == '?';
}

char to_upper_ascii(char symbol) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(symbol)));
}

std::string trim_whitespace(std::string_view input) {
    std::size_t begin = 0;
    while (begin < input.size() && is_space(input[begin])) {
        ++begin;
    }

    std::size_t end = input.size();
    while (end > begin && is_space(input[end - 1])) {
        --end;
    }

    return std::string(input.substr(begin, end - begin));
}

std::vector<std::string> split_into_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string current;

    for (std::size_t idx = 0; idx < text.size(); ++idx) {
        const char ch = text[idx];
        current.push_back(ch);

        if (is_sentence_terminator(ch)) {
            while (idx + 1 < text.size() && is_sentence_terminator(text[idx + 1])) {
                ++idx;
                current.push_back(text[idx]);
            }
            std::string cleaned = trim_whitespace(current);
            if (!cleaned.empty()) {
                sentences.push_back(std::move(cleaned));
            }
            current.clear();
        }
    }

    std::string tail = trim_whitespace(current);
    if (!tail.empty()) {
        sentences.push_back(std::move(tail));
    }

    return sentences;
}

}  // namespace

TextEditor::TextEditor(std::string initial_text) : content_(std::move(initial_text)) {}

const std::string& TextEditor::content() const noexcept {
    return content_;
}

std::string TextEditor::replaceWord(const std::string& target,
                                    const std::string& replacement) const {
    if (target.empty()) {
        return content_;
    }

    std::string output;
    output.reserve(content_.size());

    std::size_t cursor = 0;
    while (cursor < content_.size()) {
        const std::size_t match = content_.find(target, cursor);
        if (match == std::string::npos) {
            output.append(content_, cursor, std::string::npos);
            break;
        }

        const bool left_ok = (match == 0) || !is_word_char(content_[match - 1]);
        const std::size_t after_match = match + target.size();
        const bool right_ok =
            (after_match == content_.size()) || !is_word_char(content_[after_match]);

        if (left_ok && right_ok) {
            output.append(content_, cursor, match - cursor);
            output.append(replacement);
            cursor = after_match;
        } else {
            output.append(content_, cursor, match - cursor + 1);
            cursor = match + 1;
        }
    }

    return output;
}

std::string TextEditor::searchPhrase(const std::string& phrase) const {
    if (phrase.empty()) {
        return "Not found";
    }
    const bool exists = content_.find(phrase) != std::string::npos;
    return exists ? "Found" : "Not found";
}

std::string TextEditor::capitalizeEachWord() const {
    std::string output = content_;
    bool inside_word = false;

    for (char& ch : output) {
        if (is_word_char(ch)) {
            if (!inside_word) {
                ch = to_upper_ascii(ch);
                inside_word = true;
            }
        } else {
            inside_word = false;
        }
    }

    return output;
}

std::string TextEditor::capitalizeSentences() const {
    std::string output = content_;
    bool awaiting_first_letter = true;

    for (char& ch : output) {
        if (is_sentence_terminator(ch)) {
            awaiting_first_letter = true;
            continue;
        }
        if (awaiting_first_letter && is_letter(ch)) {
            ch = to_upper_ascii(ch);
            awaiting_first_letter = false;
        }
    }

    return output;
}

std::string TextEditor::formatNumberedSentences() const {
    const std::vector<std::string> sentences = split_into_sentences(content_);
    std::ostringstream buffer;

    for (std::size_t idx = 0; idx < sentences.size(); ++idx) {
        if (idx > 0) {
            buffer << '\n';
        }
        buffer << (idx + 1) << ". " << sentences[idx];
    }

    return buffer.str();
}
