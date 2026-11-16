#pragma once

#include <cctype>
#include <string>
#include <utility>

class TextDocument {
private:
    static bool is_word_char(char ch);
    std::string m_text;

public:
    explicit TextDocument(std::string text = {});
    const std::string& get_text() const;

    std::string replace_words(const std::string& old_word, const std::string& new_word);
    std::string find_phrase(const std::string& phrase) const;
    std::string capitalize_words();
    std::string capitalize_sentences();
    std::string number_sentences();
}; 
