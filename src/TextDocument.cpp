#include "TextDocument.h"
#include <cctype>
#include <utility>

bool TextDocument::is_word_char(char character) {
    const auto char_value = static_cast<unsigned char>(character);
    return (std::isalnum(char_value) != 0) || character == '_';
}

TextDocument::TextDocument(std::string text) : m_text(std::move(text)) {}

const std::string& TextDocument::get_text() const {
    return m_text;
}

std::string TextDocument::replace_words(const std::string& old_word, const std::string& new_word) {
    if (old_word.empty()) {
        return m_text;
    }

    std::size_t pos = 0;
    while ((pos = m_text.find(old_word, pos)) != std::string::npos) {
        const bool left_boundary = pos == 0 || !is_word_char(m_text[pos - 1]);
        const std::size_t end = pos + old_word.size();
        const bool right_boundary = end == m_text.size() || !is_word_char(m_text[end]);
        if (left_boundary && right_boundary) {
            m_text.replace(pos, old_word.size(), new_word);
            pos += new_word.size();
        }
        else {
            pos += old_word.size();
        }
    }
    return m_text;
}

std::string TextDocument::find_phrase(const std::string& phrase) const {
    if (phrase.empty()) {
        return "Phrase is empty";
    }
    std::string result;
    std::size_t pos = m_text.find(phrase);
    while (pos != std::string::npos) {
        if (!result.empty()) {
            result += ", ";
        }
        result += std::to_string(pos);
        pos = m_text.find(phrase, pos + 1);
    }
    return result.empty() ? "Not found" : result;
}

std::string TextDocument::capitalize_words() {
    bool beginning_of_word = true;
    for (char& character : m_text) {
        const auto char_value = static_cast<unsigned char>(character);
        if (is_word_char(character)) {
            if (beginning_of_word) {
                character = static_cast<char>(std::toupper(char_value));
            }
            beginning_of_word = false;
        }
        else {
            beginning_of_word = true;
        }
    }
    return m_text;
}

std::string TextDocument::capitalize_sentences() {
    bool need_capital = true;
    for (char& character : m_text) {
        const auto char_value = static_cast<unsigned char>(character);
        if (need_capital && (std::isalpha(char_value) != 0)) {
            character = static_cast<char>(std::toupper(char_value));
            need_capital = false;
        }
        if (character == '.' || character == '!' || character == '?') {
            need_capital = true;
        }
    }
    return m_text;
}

std::string TextDocument::number_sentences() {
    std::string result;
    std::size_t sentence_number = 1;
    bool at_sentence_start = true;

    for (std::size_t i = 0; i < m_text.size(); ++i) {
        const char character = m_text[i];
        if (at_sentence_start && (std::isspace(static_cast<unsigned char>(character)) == 0)) {
            if (!result.empty()) {
                result += '\n';
            }
            result += std::to_string(sentence_number++) + ". ";
            at_sentence_start = false;
        }
        result += character;
        if (character == '.' || character == '!' || character == '?') {
            std::size_t next_pos = i + 1;
            while (next_pos < m_text.size() &&
                (m_text[next_pos] == '.' || m_text[next_pos] == '!' || m_text[next_pos] == '?')) {
                result += m_text[next_pos++];
            }
            i = next_pos - 1;
            at_sentence_start = true;
        }
    }
    m_text = std::move(result);
    return m_text;
}