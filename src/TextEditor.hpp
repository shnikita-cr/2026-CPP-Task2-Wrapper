#pragma once

#include <string>

class TextEditor {
public:
    explicit TextEditor(std::string initial_text);

    const std::string& content() const noexcept;

    std::string replaceWord(const std::string& target, const std::string& replacement) const;
    std::string searchPhrase(const std::string& phrase) const;
    std::string capitalizeEachWord() const;
    std::string capitalizeSentences() const;
    std::string formatNumberedSentences() const;

private:
    std::string content_;
};
