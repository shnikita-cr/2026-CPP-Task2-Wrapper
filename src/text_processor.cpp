#include "text_processor.h"
#include <cctype>
#include <cstddef>
#include <string>
#include <vector>
#include "string_utils.h"

std::string TextProcessor::replace_words(std::string text, const std::string& target, const std::string& replacement) {
    if (target.empty()) {
        return text;
    }
    std::size_t pos = 0;
    while ((pos = text.find(target, pos)) != std::string::npos) {
        text.replace(pos, target.size(), replacement);
        pos += replacement.size();
    }
    return text;
}

std::string TextProcessor::find_phrase(const std::string& text, const std::string& phrase) {
    return text.find(phrase) != std::string::npos ? "found" : "not found";
}

std::string TextProcessor::capitalize_words(std::string text) {
    bool at_word_start = true;
    for (char& symbol : text) {
        const auto uchar = static_cast<unsigned char>(symbol);
        if (std::isspace(uchar) != 0) {
            at_word_start = true;
        } else {
            if (at_word_start && (std::isalpha(uchar) != 0)) {
                symbol = static_cast<char>(std::toupper(uchar));
            }
            at_word_start = false;
        }
    }
    return text;
}

std::string TextProcessor::capitalize_sentences(std::string text) {
    bool at_sentence_start = true;
    for (char& symbol : text) {
        const auto uchar = static_cast<unsigned char>(symbol);
        if (at_sentence_start && (std::isalpha(uchar) != 0)) {
            symbol = static_cast<char>(std::toupper(uchar));
            at_sentence_start = false;
        } else if (symbol == '.' || symbol == '!' || symbol == '?') {
            at_sentence_start = true;
        } else if (std::isspace(uchar) == 0) {
            at_sentence_start = false;
        }
    }
    return text;
}

std::string TextProcessor::number_sentences(const std::string& text) {
    const std::vector<std::string> sentences = utils::split_sentences(text);
    std::string result;
    for (std::size_t i = 0; i < sentences.size(); ++i) {
        std::size_t start = sentences[i].find_first_not_of(' ');
        std::string clean_sentence = (start == std::string::npos) ? "" : sentences[i].substr(start);

        result += std::to_string(i + 1) + ". " + clean_sentence;
        if (i + 1 < sentences.size()) {
            result += '\n';
        }
    }
    return result;
}