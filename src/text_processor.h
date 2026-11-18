#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <cctype>
#include <string>

class TextProcessor {
public:
    std::string replace_word(std::string text, const std::string& old_word,
                             const std::string& new_word) const {
        if (old_word.empty()) {
            return text;
        }
        std::size_t position = 0;
        while ((position = text.find(old_word, position)) != std::string::npos) {
            const std::size_t end = position + old_word.size();
            const bool starts_inside_word =
                is_word_before(text, position) &&
                is_word_byte(static_cast<unsigned char>(old_word.front()));
            const bool ends_inside_word =
                is_word_at(text, end) &&
                is_word_byte(static_cast<unsigned char>(old_word.back()));
            if (starts_inside_word || ends_inside_word) {
                position = end;
                continue;
            }
            text.replace(position, old_word.size(), new_word);
            position += new_word.size();
        }
        return text;
    }

    std::string find_phrase(const std::string& text, const std::string& phrase) const {
        return text.find(phrase) == std::string::npos ? "не найдено" : "найдено";
    }

    std::string capitalize_words(std::string text) const {
        bool capitalize_next = true;
        for (std::size_t index = 0; index < text.size(); ++index) {
            const std::size_t quote_width = utf8_quote_width_at(text, index);
            if (quote_width > 0) {
                capitalize_next = true;
                index += quote_width - 1;
                continue;
            }
            const unsigned char current = static_cast<unsigned char>(text[index]);
            if (is_word_byte(current)) {
                if (capitalize_next) {
                    capitalize_utf8_letter(text, index);
                    capitalize_next = false;
                }
            } else {
                capitalize_next = true;
            }
        }
        return text;
    }

    std::string capitalize_sentences(std::string text) const {
        bool capitalize_next = true;
        for (std::size_t index = 0; index < text.size(); ++index) {
            const std::size_t quote_width = utf8_quote_width_at(text, index);
            if (quote_width > 0) {
                index += quote_width - 1;
                continue;
            }
            const unsigned char current = static_cast<unsigned char>(text[index]);
            if (capitalize_next && is_word_byte(current)) {
                capitalize_utf8_letter(text, index);
                capitalize_next = false;
            }
            if (is_sentence_ending(text, index)) {
                capitalize_next = true;
            }
        }
        return text;
    }

    std::string number_sentences(const std::string& text) const {
        std::string result;
        std::string sentence;
        std::size_t sentence_number = 1;
        for (std::size_t index = 0; index < text.size(); ++index) {
            const char character = text[index];
            if (character == '\n' || character == '\r') {
                append_sentence(result, sentence, sentence_number);
                continue;
            }
            sentence.push_back(character);
            if (is_sentence_ending(text, index)) {
                while (index + 1 < text.size() && is_sentence_mark(text[index + 1])) {
                    sentence.push_back(text[++index]);
                }
                while (index + 1 < text.size() && is_closing_punctuation(text[index + 1])) {
                    sentence.push_back(text[++index]);
                }
                while (index + 1 < text.size()) {
                    const std::size_t quote_width = utf8_closing_quote_width_at(text, index + 1);
                    if (quote_width == 0) {
                        break;
                    }
                    sentence.append(text, index + 1, quote_width);
                    index += quote_width;
                }
                append_sentence(result, sentence, sentence_number);
            }
        }
        append_sentence(result, sentence, sentence_number);
        return result;
    }

private:
    static std::size_t utf8_quote_width_at(const std::string& text, std::size_t index) {
        if (index + 1 < text.size() && static_cast<unsigned char>(text[index]) == 0xC2U &&
            (static_cast<unsigned char>(text[index + 1]) == 0xABU ||
             static_cast<unsigned char>(text[index + 1]) == 0xBBU)) {
            return 2;
        }
        if (index + 2 < text.size() && static_cast<unsigned char>(text[index]) == 0xE2U &&
            static_cast<unsigned char>(text[index + 1]) == 0x80U) {
            const unsigned char last = static_cast<unsigned char>(text[index + 2]);
            if (last == 0x98U || last == 0x99U || last == 0x9CU || last == 0x9DU) {
                return 3;
            }
        }
        return 0;
    }

    static std::size_t utf8_closing_quote_width_at(const std::string& text,
                                                    std::size_t index) {
        if (utf8_quote_width_at(text, index) == 2 &&
            static_cast<unsigned char>(text[index + 1]) == 0xBBU) {
            return 2;
        }
        if (index + 2 < text.size() && static_cast<unsigned char>(text[index]) == 0xE2U &&
            static_cast<unsigned char>(text[index + 1]) == 0x80U &&
            (static_cast<unsigned char>(text[index + 2]) == 0x99U ||
             static_cast<unsigned char>(text[index + 2]) == 0x9DU)) {
            return 3;
        }
        return 0;
    }

    static bool is_word_at(const std::string& text, std::size_t index) {
        return index < text.size() && utf8_quote_width_at(text, index) == 0 &&
               is_word_byte(static_cast<unsigned char>(text[index]));
    }

    static bool is_word_before(const std::string& text, std::size_t index) {
        if (index == 0) {
            return false;
        }
        if (index >= 2 && utf8_quote_width_at(text, index - 2) == 2) {
            return false;
        }
        if (index >= 3 && utf8_quote_width_at(text, index - 3) == 3) {
            return false;
        }
        return is_word_byte(static_cast<unsigned char>(text[index - 1]));
    }

    static bool is_sentence_mark(char character) {
        return character == '.' || character == '!' || character == '?';
    }

    static bool is_sentence_ending(const std::string& text, std::size_t index) {
        if (!is_sentence_mark(text[index])) {
            return false;
        }
        if (text[index] != '.' || index == 0 || index + 1 >= text.size()) {
            return true;
        }
        const unsigned char previous = static_cast<unsigned char>(text[index - 1]);
        const unsigned char next = static_cast<unsigned char>(text[index + 1]);
        return std::isdigit(previous) == 0 || std::isdigit(next) == 0;
    }

    static bool is_closing_punctuation(char character) {
        return character == '"' || character == '\'' || character == ')' ||
               character == ']' || character == '}';
    }

    static bool is_word_byte(unsigned char character) {
        return std::isalnum(character) != 0 || character >= 0x80U;
    }

    static void capitalize_utf8_letter(std::string& text, std::size_t index) {
        const unsigned char first = static_cast<unsigned char>(text[index]);
        if (first >= 'a' && first <= 'z') {
            text[index] = static_cast<char>(first - ('a' - 'A'));
            return;
        }
        if (index + 1 < text.size() && first == 0xD0U) {
            const unsigned char second = static_cast<unsigned char>(text[index + 1]);
            if (second >= 0xB0U && second <= 0xBFU) {
                text[index + 1] = static_cast<char>(second - 0x20U);
            } else if (second == 0xB1U) {
                text[index] = static_cast<char>(0xD0U);
                text[index + 1] = static_cast<char>(0x81U);
            }
            return;
        }
        if (index + 1 >= text.size() || first != 0xD1U) {
            return;
        }

        const unsigned char second = static_cast<unsigned char>(text[index + 1]);
        if (second >= 0x80U && second <= 0x8FU) {
            text[index] = static_cast<char>(0xD0U);
            text[index + 1] = static_cast<char>(second + 0x20U);
        } else if (second == 0x91U) {
            text[index] = static_cast<char>(0xD0U);
            text[index + 1] = static_cast<char>(0x81U);
        }
    }

    static void append_sentence(std::string& result, std::string& sentence,
                                std::size_t& sentence_number) {
        std::size_t first = 0;
        while (first < sentence.size() &&
               std::isspace(static_cast<unsigned char>(sentence[first])) != 0) {
            ++first;
        }
        std::size_t last = sentence.size();
        while (last > first &&
               std::isspace(static_cast<unsigned char>(sentence[last - 1])) != 0) {
            --last;
        }
        if (first < last) {
            if (!result.empty()) {
                result.push_back('\n');
            }
            result += std::to_string(sentence_number++);
            result += ". ";
            result.append(sentence, first, last - first);
        }
        sentence.clear();
    }
};

#endif
