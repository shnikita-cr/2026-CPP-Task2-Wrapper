#pragma once
#include <string>

class textdoc {
public:
    explicit textdoc(std::string text);

    std::string replaceWord(const std::string& target, const std::string& replacement) const;
    std::string findPhrase(const std::string& phrase) const;
    std::string capitalizeWords() const;
    std::string capitalizeSentences() const;
    std::string numberSentences() const;

    const std::string& getText() const;

private:
    std::string text_;
};