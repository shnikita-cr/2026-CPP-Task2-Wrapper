#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <string>
#include <cctype>
#include <sstream>
#include <vector>

class TextProcessor {
public:
    std::string replaceWords(const std::string& text, const std::string& from, const std::string& to) {
        if (from.empty()) {
            return text;
        }

        auto isWordChar = [](char c) {
            return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
            };

        std::string result;
        result.reserve(text.size());
        std::size_t pos = 0;

        while (pos < text.size()) {
            std::size_t found = text.find(from, pos);
            if (found == std::string::npos) {
                result.append(text, pos, std::string::npos);
                break;
            }

            bool leftOk = (found == 0) || !isWordChar(text[found - 1]);
            bool rightOk = (found + from.size() == text.size()) || !isWordChar(text[found + from.size()]);

            if (leftOk && rightOk) {
                result.append(text, pos, found - pos);
                result += to;
                pos = found + from.size();
            }
            else {
                result.append(text, pos, found - pos + 1);
                pos = found + 1;
            }
        }
        return result;
    }

    std::string findPhrase(const std::string& text, const std::string& phrase) {
        if (phrase.empty()) {
            return "not found";
        }
        return text.find(phrase) != std::string::npos ? "found" : "not found";
    }

    std::string capitalizeWords(const std::string& text) {
        std::string result = text;
        bool newWord = true;
        for (char& c : result) {
            if (std::isspace(static_cast<unsigned char>(c))) {
                newWord = true;
            }
            else if (newWord && std::isalpha(static_cast<unsigned char>(c))) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                newWord = false;
            }
            else {
                newWord = false;
            }
        }
        return result;
    }

    std::string capitalizeSentences(const std::string& text) {
        std::string result = text;
        bool newSentence = true;
        for (char& c : result) {
            if (newSentence && std::isalpha(static_cast<unsigned char>(c))) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                newSentence = false;
            }
            else if (c == '.' || c == '!' || c == '?') {
                newSentence = true;
            }
        }
        return result;
    }

    std::string numberSentences(const std::string& text) {
        std::string result;
        std::size_t start = 0;
        std::size_t counter = 1;
        while (start < text.size()) {
            std::size_t end = start;
            while (end < text.size() && text[end] != '.' && text[end] != '!' && text[end] != '?') {
                ++end;
            }
            if (end < text.size()) {
                ++end;
            }
            std::string sentence = text.substr(start, end - start);
            std::size_t firstNonSpace = sentence.find_first_not_of(" \t\n");
            if (firstNonSpace != std::string::npos) {
                sentence = sentence.substr(firstNonSpace);
            }
            if (!sentence.empty()) {
                result += std::to_string(counter) + ". " + sentence + "\n";
                ++counter;
            }
            start = end;
        }
        return result;
    }
};

#endif