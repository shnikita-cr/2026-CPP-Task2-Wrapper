#pragma once

#include <string>
#include <string_view>

class TextEditor {
public:
    explicit TextEditor(std::string initial_text);

    std::string_view content() const noexcept;

    std::string replaceWord(const std::string& target, const std::string& replacement) const;
    std::string searchPhrase(const std::string& phrase) const;
    std::string capitalizeEachWord() const;
    std::string capitalizeSentences() const;
    std::string formatNumberedSentences() const;

private:
    std::string content_;
};
