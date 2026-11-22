#pragma once

#include "IWrapper.h"

#include <cctype>
#include <sstream>
#include <string>

class TextDocument {
public:
    explicit TextDocument(std::string text)
        : m_text(std::move(text)) {
    }

    const std::string& getText() const { return m_text; }

    std::string replaceWord(const Arguments& args) {
        const std::string& oldWord = args.at("oldWord");
        const std::string& newWord = args.at("newWord");
        std::size_t pos = 0;
        while ((pos = m_text.find(oldWord, pos)) != std::string::npos) {
            m_text.replace(pos, oldWord.length(), newWord);
            pos += newWord.length();
        }
        return m_text;
    }

    std::string findPhrase(const Arguments& args) {
        const std::string& phrase = args.at("phrase");
        std::size_t pos = m_text.find(phrase);
        if (pos != std::string::npos) {
            return "Found at index: " + std::to_string(pos);
        }
        return "Not found";
    }

    std::string capitalizeWords([[maybe_unused]] const Arguments& args) {
        bool capitalizeNext = true;
        for (char& c : m_text) {
            if (std::isspace(static_cast<unsigned char>(c))) {
                capitalizeNext = true;
            }
            else if (capitalizeNext) {
                c = static_cast<char>(
                    std::toupper(static_cast<unsigned char>(c)));
                capitalizeNext = false;
            }
        }
        return m_text;
    }

    std::string capitalizeSentences(
        [[maybe_unused]] const Arguments& args) {
        bool capitalizeNext = true;
        for (char& c : m_text) {
            if (capitalizeNext &&
                std::isalpha(static_cast<unsigned char>(c))) {
                c = static_cast<char>(
                    std::toupper(static_cast<unsigned char>(c)));
                capitalizeNext = false;
            }
            else if (c == '.' || c == '!' || c == '?') {
                capitalizeNext = true;
            }
        }
        return m_text;
    }

    std::string numberSentences(
        [[maybe_unused]] const Arguments& args) {
        std::string result;
        std::istringstream stream(m_text);
        std::string line;
        int num = 1;
        while (std::getline(stream, line)) {
            if (!line.empty()) {
                result += std::to_string(num) + ". " + line + "\n";
                ++num;
            }
            else {
                result += "\n";
            }
        }
        m_text = result;
        return m_text;
    }

private:
    std::string m_text;
};