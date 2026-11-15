#pragma once

#include <string>
#include <cctype>

class Formatter {
public:
    std::string replaceWords(const std::string& text, const std::string& from, const std::string& too);

    bool isStringInText(const std::string& text, const std::string& request);

    std::string eachCapitalize(const std::string& text);

    std::string firstCapitalize(const std::string& text);

    std::string enumerateSentences(const std::string& text);
};