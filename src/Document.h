#pragma once

#include "Wrapper.h"

#include <string>

class Document {
public:
    explicit Document(std::string text = "");

    [[nodiscard]] const std::string& text() const;

    std::string replace_words(const Arguments& args);
    [[nodiscard]] std::string find_phrase(const Arguments& args) const;
    std::string capitalize_words(const Arguments& args);
    std::string capitalize_sentences(const Arguments& args);
    std::string number_sentences(const Arguments& args);

private:
    std::string text_;
};
