//
//  text_document.h
//  Wrapper
//
//  Created by Богдан on 08.11.2026.
//

#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include "any_map.h"

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

class TextDocument {
public:
    explicit TextDocument(std::string text = "") : text_(std::move(text)) {}

    const std::string& content() const { return text_; }

    void set_content(std::string text) { text_ = std::move(text); }

    std::string replace_words(const AnyMap& args) {
        const std::string old_w = get_arg<std::string>(args, "old");
        const std::string new_w = get_arg<std::string>(args, "new");
        return replace_words(old_w, new_w);
    }

    std::string replace_words(const std::string& old_word, const std::string& new_word) {
        if (old_word.empty()) {
            return text_;
        }
        std::string result;
        result.reserve(text_.size());
        size_t pos = 0;
        while (true) {
            const size_t found = text_.find(old_word, pos);
            if (found == std::string::npos) {
                result.append(text_, pos, std::string::npos);
                break;
            }
            result.append(text_, pos, found - pos);
            result.append(new_word);
            pos = found + old_word.size();
        }
        text_ = std::move(result);
        return text_;
    }

    std::string search_phrase(const AnyMap& args) {
        const std::string phrase = get_arg<std::string>(args, "phrase");
        return search_phrase(phrase);
    }

    std::string search_phrase(const std::string& phrase) {
        if (phrase.empty()) {
            return "empty phrase";
        }
        std::ostringstream oss;
        size_t pos = 0;
        int count = 0;
        while (true) {
            const size_t found = text_.find(phrase, pos);
            if (found == std::string::npos) {
                break;
            }
            if (count > 0) {
                oss << ", ";
            }
            oss << found;
            ++count;
            pos = found + 1;
        }
        if (count == 0) {
            return "not found";
        }
        return "found " + std::to_string(count) + " time(s) at: " + oss.str();
    }

    std::string capitalize_words(const AnyMap& /*args*/) {
        return capitalize_words_impl();
    }

    std::string capitalize_words() { return capitalize_words_impl(); }

    std::string capitalize_sentences(const AnyMap& /*args*/) {
        return capitalize_sentences_impl();
    }

    std::string capitalize_sentences() { return capitalize_sentences_impl(); }

    std::string number_sentences(const AnyMap& args) {
        const int start = get_arg_or<int>(args, "start", 1);
        return number_sentences(start);
    }

    std::string number_sentences(int start = 1) {
        std::vector<std::string> sentences;
        std::string current;
        for (size_t i = 0; i < text_.size(); ++i) {
            const char c = text_[i];
            current += c;
            if (c == '.' || c == '!' || c == '?') {
                const std::string s = trim(current);
                if (!s.empty()) {
                    sentences.push_back(s);
                }
                current.clear();
            }
        }
        const std::string tail = trim(current);
        if (!tail.empty()) {
            sentences.push_back(tail);
        }

        std::ostringstream oss;
        int num = start;
        for (size_t i = 0; i < sentences.size(); ++i) {
            if (i > 0) {
                oss << '\n';
            }
            oss << num << ". " << sentences[i];
            ++num;
        }
        text_ = oss.str();
        return text_;
    }

private:
    std::string capitalize_words_impl() {
        bool new_word = true;
        for (char& c : text_) {
            if (std::isspace(static_cast<unsigned char>(c)) ||
                std::ispunct(static_cast<unsigned char>(c))) {
                new_word = true;
            } else if (new_word) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                new_word = false;
            }
        }
        return text_;
    }

    std::string capitalize_sentences_impl() {
        bool new_sentence = true;
        for (size_t i = 0; i < text_.size(); ++i) {
            char& c = text_[i];
            if (new_sentence && std::isalpha(static_cast<unsigned char>(c))) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                new_sentence = false;
            } else if (c == '.' || c == '!' || c == '?') {
                new_sentence = true;
            }
        }
        return text_;
    }

    static std::string trim(const std::string& s) {
        size_t begin = 0;
        while (begin < s.size() &&
               std::isspace(static_cast<unsigned char>(s[begin]))) {
            ++begin;
        }
        size_t end = s.size();
        while (end > begin &&
               std::isspace(static_cast<unsigned char>(s[end - 1]))) {
            --end;
        }
        return s.substr(begin, end - begin);
    }

    std::string text_;
};

#endif  // TEXT_DOCUMENT_H
