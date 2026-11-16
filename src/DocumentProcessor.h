#pragma once

#include <cctype>
#include <cstddef>
#include <sstream>
#include <string>
#include <utility>

class DocumentProcessor {
public:
    explicit DocumentProcessor(std::string text) : text_(std::move(text)) {}
    [[nodiscard]] const std::string& get_text() const {
        return text_;
    }
    std::string replace_word(const std::string& target, const std::string& replacement) {
        if (target.empty()) {
            return text_;
        }
        std::string result;
        result.reserve(text_.size());
        std::size_t pos = 0;
        while (pos < text_.size()) {
            const std::size_t found = text_.find(target, pos);
            if (found == std::string::npos) {
                result.append(text_, pos, std::string::npos);
                break;
            }
            result.append(text_, pos, found - pos);
            const bool left_ok = (found == 0) || is_word_boundary(found - 1);
            const bool right_ok = is_word_boundary(found + target.size());

            if (left_ok && right_ok) {
                result.append(replacement);
            } else {
                result.append(target);
            }
            pos = found + target.size();
        }

        text_ = std::move(result);
        return text_;
    }
    [[nodiscard]] std::string find_phrase(const std::string& phrase) const {
        if (phrase.empty()) {
            return "не найдено";
        }
        const bool found = text_.find(phrase) != std::string::npos;
        return found ? "найдено" : "не найдено";
    }
    std::string capitalize_words() {
        bool new_word = true;
        for (char& ch : text_) {
            const auto uch = static_cast<unsigned char>(ch);
            if (std::isspace(uch) != 0 || std::ispunct(uch) != 0) {
                new_word = true;
            } else if (new_word && std::isalpha(uch) != 0) {
                ch = static_cast<char>(std::toupper(uch));
                new_word = false;
            } else {
                new_word = false;
            }
        }
        return text_;
    }
    std::string capitalize_sentences() {
        bool new_sentence = true;
        for (char& ch : text_) {
            const auto uch = static_cast<unsigned char>(ch);
            if (new_sentence && std::isalpha(uch) != 0) {
                ch = static_cast<char>(std::toupper(uch));
                new_sentence = false;
            } else if (ch == '.' || ch == '!' || ch == '?') {
                new_sentence = true;
            }
        }
        return text_;
    }
    std::string number_sentences() {
        std::ostringstream oss;
        int sentence_num = 1;
        std::size_t start = 0;
        while (start < text_.size()) {
            while (start < text_.size() && std::isspace(static_cast<unsigned char>(text_[start])) != 0) {
                ++start;
            }
            if (start >= text_.size()) {
                break;
            }
            std::size_t end = start;
            while (end < text_.size() && text_[end] != '.' && text_[end] != '!' && text_[end] != '?') {
                ++end;
            }
            if (end < text_.size()) {
                ++end;
            }

            if (sentence_num > 1) {
                oss << '\n';
            }
            oss << sentence_num << ". " << text_.substr(start, end - start);
            ++sentence_num;
            start = end;
        }
        text_ = oss.str();
        return text_;
    }
private:
    std::string text_;
    [[nodiscard]] bool is_word_boundary(std::size_t index) const {
        if (index >= text_.size()) {
            return true;
        }
        const auto ch = static_cast<unsigned char>(text_[index]);
        return std::isalnum(ch) == 0;
    }
};
