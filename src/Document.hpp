#pragma once
#include <string>

class Document {
public:
    explicit Document(std::string text);

    std::string replace_words(const std::string &from, const std::string& to);
    bool find_phrase(const std::string& phrase);
    std::string capitalize_words();
    std::string capitalize_sentences();
    std::string number_sentences();

private:
    std::string text_;
};