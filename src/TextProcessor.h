#pragma once
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

class TextProcessor
{
public:
    [[maybe_unused]] explicit TextProcessor(std::string text) : text_(std::move(text))
    {}

    [[nodiscard]] std::string replace_words(const std::string& from, const std::string& p_to) const;

    [[nodiscard]] std::string find_phrase(const std::string& phrase) const;

    [[nodiscard]] std::string capitalize_words() const;

    [[nodiscard]] std::string capitalize_sentences() const;

    [[nodiscard]] std::string number_sentences() const;

private:
    static std::string trim(const std::string& strng)
    {
        const auto first = strng.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            return {};
        }
        const auto last = strng.find_last_not_of(" \t\r\n");
        return strng.substr(first, last - first + 1);
    }

    std::string text_;
};