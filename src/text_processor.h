#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include "wrapper.h"

#include <string>
#include <cctype>

class TextProcessor
{
private:
    std::string text;

    // Проверка, является ли символ буквой Windows-1251.
    static bool is_letter(const std::string& str, size_t pos) {
        if (pos >= str.size()) { return false; }

        unsigned char c = static_cast<unsigned char>(str[pos]);

        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) { return true; }
        if (c >= 0xC0 || c == 0xA8 || c == 0xB8) { return true; }

        return false;
    }

    // Преобразование буквы в верхний регистр.
    static void uppercase(std::string& str, size_t pos) {
        if (pos >= str.size()) { return; }

        unsigned char c = static_cast<unsigned char>(str[pos]);

        if (c >= 'a' && c <= 'z') {
            str[pos] = static_cast<char>(c - 32);
            return;
        }

        if (c >= 0xE0) {
            str[pos] = static_cast<char>(c - 0x20);
            return;
        }

        // буква ё.
        if (c == 0xB8) { str[pos] = static_cast<char>(0xA8); }
    }

    // Удаление пробелов в начале и конце строки.
    static std::string trim(const std::string& str) {
        const std::string spaces = " \t\n\r";

        size_t first = str.find_first_not_of(spaces);

        if (first == std::string::npos) { return ""; }

        size_t last = str.find_last_not_of(spaces);

        return str.substr(first, last - first + 1);
    }

public:
    explicit TextProcessor(const std::string& document) : text(document) {}

    // 1. Замена слов.
    std::string replace_words(const Arguments& args) {
        std::string old_word = args.at("old_word");
        std::string new_word = args.at("new_word");

        if (old_word.empty()) { return text; }

        size_t pos = 0;

        while ((pos = text.find(old_word, pos)) != std::string::npos) {
            bool left_boundary = pos == 0 || !is_letter(text, pos - 1);

            size_t right_pos = pos + old_word.size();

            bool right_boundary = right_pos >= text.size() || !is_letter(text, right_pos);

            if (left_boundary && right_boundary) {
                text.replace(pos, old_word.size(), new_word);
                pos += new_word.size();
            }
            else { pos += old_word.size(); }
        }

        return text;
    }

    // 2. Поиск словосочетания.
    std::string find_phrase(const Arguments& args) {
        std::string phrase = args.at("phrase");

        if (text.find(phrase) != std::string::npos) { return "\xD1\xEB\xEE\xE2\xEE\xF1\xEE\xF7\xE5\xF2\xE0\xED\xE8\xE5 \xED\xE0\xE9\xE4\xE5\xED\xEE"; }

        return "\xD1\xEB\xEE\xE2\xEE\xF1\xEE\xF7\xE5\xF2\xE0\xED\xE8\xE5 \xED\xE5 \xED\xE0\xE9\xE4\xE5\xED\xEE";
    }

    // 3. Заглавные первые буквы слов.
    std::string capitalize_words(const Arguments&) {
        bool new_word = true;

        for (size_t i = 0; i < text.size(); ++i) {
            if (is_letter(text, i)) {
                if (new_word) { uppercase(text, i); }

                new_word = false;
            }
            else { new_word = true; }
        }

        return text;
    }

    // 4. Заглавные первые буквы предложений.
    std::string capitalize_sentences(const Arguments&) {
        bool new_sentence = true;

        for (size_t i = 0; i < text.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(text[i]);

            if (new_sentence && std::isspace(c)) { continue; }

            if (new_sentence && is_letter(text, i)) {
                uppercase(text, i);
                new_sentence = false;
            }

            if (c == '.' || c == '!' || c == '?') { new_sentence = true; }
        }

        return text;
    }

    // 5. Нумерация предложений с новой строки.
    std::string number_sentences(const Arguments&)
    {
        std::string result;
        std::string sentence;

        int number = 1;

        for (size_t i = 0; i < text.size(); ++i) {
            sentence += text[i];

            if (text[i] == '.' || text[i] == '!' || text[i] == '?') {
                while (i + 1 < text.size() && (text[i + 1] == '.' || text[i + 1] == '!' || text[i + 1] == '?')) {
                    sentence += text[++i];
                }

                sentence = trim(sentence);

                if (!sentence.empty()) {
                    if (!result.empty()) { result += '\n'; }

                    result += std::to_string(number++) + ". " + sentence;
                }

                sentence.clear();
            }
        }

        sentence = trim(sentence);

        if (!sentence.empty()) {
            if (!result.empty()) { result += '\n'; }
            result += std::to_string(number) + ". " + sentence;
        }

        text = result;

        return text;
    }
};

#endif





