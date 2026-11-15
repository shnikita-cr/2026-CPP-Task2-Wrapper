#include "Formatter.h"

std::string Formatter::replaceWords(const std::string& text, const std::string& from, const std::string& too) { // NOLINT(readability-convert-member-functions-to-static, bugprone-easily-swappable-parameters)
    if (from.empty()) {
        return text;
    }
    
    std::string newtext = text;
    size_t index = 0;
    while ((index = newtext.find(from, index)) != std::string::npos) {
        newtext.replace(index, from.length(), too);
        index += too.length(); 
    }

    return newtext;
}

bool Formatter::isStringInText(const std::string& text, const std::string& request) { // NOLINT(readability-convert-member-functions-to-static, bugprone-easily-swappable-parameters)
    return text.find(request) != std::string::npos;
}

std::string Formatter::eachCapitalize(const std::string& text) { // NOLINT(readability-convert-member-functions-to-static)
    std::string newtext = text;
    bool newWord = true;

    for (char& ccc : newtext) {
        if (static_cast<bool>(std::isspace(ccc))) {
            newWord = true;
        } else if (newWord && static_cast<bool>(std::isalpha(ccc))) {
            ccc = static_cast<char>(std::toupper(ccc));
            newWord = false;
        }
    }

    return newtext;
}

std::string Formatter::firstCapitalize(const std::string& text) { // NOLINT(readability-convert-member-functions-to-static)
    std::string newtext = text;
    bool newSent = true;

    for (char& ccc : newtext) {
        if (ccc == '.' || ccc == '!' || ccc  == '?') {
            newSent = true;
        } else if (newSent && static_cast<bool>(std::isalpha(ccc))) {
            ccc = static_cast<char>(std::toupper(ccc));
            newSent = false;
        }
    }

    return newtext;
}

std::string Formatter::enumerateSentences(const std::string& text) { // NOLINT(readability-convert-member-functions-to-static)
    std::string newtext = "1. ";
    int iii = 2;


    for (char ccc : text) {
        if (ccc == '.') {
            newtext += ".\n" + std::to_string(iii) + ".";
            iii++;
        } else {
            newtext += ccc;
        }
    }

    return newtext;
}