#include "Document.hpp"
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

Document::Document(std::string text)
    : text_(std::move(text)) {}

std::string Document::replace_words(const std::string &from, const std::string& to_str) {
    if (from.empty()) {
        return text_;
    }

    std::string result = text_;
    std::size_t pos = 0;

    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.size(), to_str);
        pos += to_str.size();
    }

    return result;
}

bool Document::find_phrase(const std::string& phrase) {
    return text_.find(phrase) != std::string::npos;
}

std::string Document::capitalize_words() {
    std::string result = text_;
    bool at_word_start = true;

    for (char& character : result) {
        auto unsigned_char = static_cast<unsigned char>(character);
        if (std::isalnum(unsigned_char) != 0) {
            if (at_word_start) {
                character = static_cast<char>(std::toupper(unsigned_char));
            }
            at_word_start = false;
        }
        else {
            at_word_start = true;
        }
    }

    return result;
}

std::string Document::capitalize_sentences() {
    std::string result = text_;
    bool sentence_start = true;

    for (char& character : result) {
        auto unsigned_char = static_cast<unsigned char>(character);
        if (sentence_start && std::isalpha(unsigned_char)!=0) {
            character = static_cast<char>(std::toupper(unsigned_char));
            sentence_start = false;
        }
        else if (character == '.' || character == '!' || character == '?') {
            sentence_start = true;
        }
    }

    return result;
}

std::string Document::number_sentences() {
    std::ostringstream out;
    out << "1. ";

    int number = 1;
    bool skip_space = false;

    for (char character : text_) {
        if (skip_space) {
            if (std::isspace(static_cast<unsigned char>(character))) {
                continue;
            }
            skip_space = false;
        }

        out << character;

        if (character == '.' || character == '!' || character == '?') {
            ++number;
            out << '\n' << number << ". ";
            skip_space = true;
        }
    }

    return out.str();
}