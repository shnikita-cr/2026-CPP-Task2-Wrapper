#include "text_processor.hpp"

#include <cctype>
#include <sstream>

std::string TextProcessor::replace_words(std::string_view text,
                                         std::string_view from,
                                         const std::string& target) {
    ++operations_count_;
    if (from.empty()) {
        return std::string{text};
    }
    std::string result;
    result.reserve(text.size());
    std::string::size_type position = 0;
    while (true) {
        const auto found = text.find(from, position);
        if (found == std::string_view::npos) {
            result.append(text.substr(position));
            break;
        }
        result.append(text.substr(position, found - position));
        result.append(target);
        position = found + from.size();
    }
    return result;
}

bool TextProcessor::find_phrase(std::string_view text,
                                std::string_view phrase) {
    ++operations_count_;
    if (phrase.empty()) {
        return false;
    }
    return text.find(phrase) != std::string_view::npos;
}

std::string TextProcessor::capitalize_words(std::string_view text) {
    ++operations_count_;
    std::string result{text};
    bool at_word_start = true;
    for (char& symbol : result) {
        const auto byte_value = static_cast<unsigned char>(symbol);
        if (std::isalpha(byte_value) != 0) {
            if (at_word_start) {
                symbol = static_cast<char>(std::toupper(byte_value));
            }
            at_word_start = false;
        } else {
            at_word_start = true;
        }
    }
    return result;
}

std::string TextProcessor::capitalize_sentences(std::string_view text) {
    ++operations_count_;
    std::string result{text};
    bool at_sentence_start = true;
    for (char& symbol : result) {
        const auto byte_value = static_cast<unsigned char>(symbol);
        if (at_sentence_start && std::isalpha(byte_value) != 0) {
            symbol = static_cast<char>(std::toupper(byte_value));
            at_sentence_start = false;
        } else if (symbol == '.' || symbol == '!' || symbol == '?') {
            at_sentence_start = true;
        }
    }
    return result;
}

std::string TextProcessor::number_sentences(std::string_view text) {
    ++operations_count_;
    const auto sentences = split_sentences(text);
    std::ostringstream out;
    std::size_t index = 1;
    for (const auto& sentence : sentences) {
        const auto first = sentence.find_first_not_of(" \t\n");
        if (first == std::string::npos) {
            continue;
        }
        out << index << ". " << sentence.substr(first) << '\n';
        ++index;
    }
    return out.str();
}

std::size_t TextProcessor::operations_count() const noexcept {
    return operations_count_;
}

std::vector<std::string> TextProcessor::split_sentences(std::string_view text) {
    std::vector<std::string> result;
    std::string current;
    for (const char symbol : text) {
        if (symbol == '\n') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
            continue;
        }
        current.push_back(symbol);
        if (symbol == '.' || symbol == '!' || symbol == '?') {
            result.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}