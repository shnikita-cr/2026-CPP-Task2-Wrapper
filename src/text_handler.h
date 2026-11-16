#pragma once

#include <cctype>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class TextHandler {
public:
    explicit TextHandler(std::string text) : m_text(std::move(text)) {}

    [[nodiscard]] std::string replace_word(std::string from, std::string to) const {
        std::string result = m_text;
        std::size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.size(), to);
            pos += to.size();
        }
        return result;
    }

    [[nodiscard]] bool find_phrase(std::string phrase) const {
        return m_text.find(phrase) != std::string::npos;
    }

    [[nodiscard]] std::string capitalize_words() const {
        std::string result = m_text;
        bool new_word = true;
        for (char& ch : result) {
            if (std::isspace(static_cast<unsigned char>(ch)) != 0) {
                new_word = true;
            }
            else if (new_word) {
                ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                new_word = false;
            }
        }
        return result;
    }

    [[nodiscard]] std::string capitalize_sentences() const {
        std::string result = m_text;
        bool new_sentence = true;
        for (char& ch : result) {
            if (new_sentence && std::isalpha(static_cast<unsigned char>(ch)) != 0) {
                ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                new_sentence = false;
            }
            else if (ch == '.' || ch == '!' || ch == '?') {
                new_sentence = true;
            }
        }
        return result;
    }

    [[nodiscard]] std::string number_sentences() const {
        std::vector<std::string> sentences;
        std::string current;
        for (char ch : m_text) {
            current += ch;
            if (ch == '.' || ch == '!' || ch == '?') {
                sentences.push_back(current);
                current.clear();
            }
        }
        if (!current.empty()) sentences.push_back(current);

        std::ostringstream oss;
        for (std::size_t i = 0; i < sentences.size(); ++i) {
            std::string sentence = sentences[i];
            std::size_t start = sentence.find_first_not_of(' ');
            if (start != std::string::npos) {
                sentence = sentence.substr(start);
            }

            oss << (i + 1) << ". " << sentence;
            if (i + 1 < sentences.size()) {
                oss << '\n';
            }
        }
        return oss.str();
    }

private:
    std::string m_text;
};