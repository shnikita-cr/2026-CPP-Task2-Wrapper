#pragma once

#include <string>
#include <utility>

class Subject {
public:
    explicit Subject(std::string text = "") : text_(std::move(text)) {}

    [[nodiscard]] const std::string& get_text() const { return text_; }
    void set_text(const std::string& text) { text_ = text; }

    std::string replace_words(const std::string& from, const std::string& replacement);
    [[nodiscard]] std::string find_phrase(const std::string& phrase) const;
    std::string capitalize_words();
    std::string capitalize_sentences();
    std::string number_sentences();

private:
    std::string text_;
};
