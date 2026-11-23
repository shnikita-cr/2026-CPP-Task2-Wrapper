#pragma once

#include <string>

class TextProcessor {
public:
    static std::string replace_words(std::string text, const std::string &target, const std::string &replacement);

    static std::string find_phrase(const std::string &text, const std::string &phrase);

    static std::string capitalize_words(std::string text);

    static std::string capitalize_sentences(std::string text);

    static std::string number_sentences(const std::string &text);
};
