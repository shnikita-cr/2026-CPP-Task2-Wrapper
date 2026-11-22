#include "Document.hpp"
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

Document::Document(std::string text)
    : text_(std::move(text)) {}

std::string Document::replace_words(const std::string &from, const std::string& to) {
    if (from.empty()) {
        return text_;
    }

    std::string result = text_;
    std::size_t pos = 0;

    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.size(), to);
        pos += to.size();
    }

    return result;
}

bool Document::find_phrase(const std::string& phrase) {
    return text_.find(phrase) != std::string::npos;
}

std::string Document::capitalize_words() {
    std::string result = text_;
    bool at_word_start = true;

    for (char& ch : result) {
        auto uc = static_cast<unsigned char>(ch);
        if (std::isalnum(uc) != 0) {
            if (at_word_start) {
                ch = static_cast<char>(std::toupper(uc));
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

    for (char& ch : result) {
        auto uc = static_cast<unsigned char>(ch);
        if (sentence_start && std::isalpha(uc)!=0) {
            ch = static_cast<char>(std::toupper(uc));
            sentence_start = false;
        }
        else if (ch == '.' || ch == '!' || ch == '?') {
            sentence_start = true;
        }
    }

    return result;
}

std::string Document::number_sentences() {
    std::ostringstream out;
    out << "1. ";

    int number = 1;
    for (std::size_t i = 0; i < text_.size(); ++i) {
        out << text_[i];

        if (text_[i] == '.' || text_[i] == '!' || text_[i] == '?') {
            ++number;
            out << '\n' << number << ". ";
        }
    }

    return out.str();
}