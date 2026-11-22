#include "Subject.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

Subject::Subject(std::string filename) : filename_(std::move(filename)) {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open file: " + filename_);
    }

    std::string line;
    while (std::getline(file, line)) {
        text_ += line + "\n";
    }

    if (!text_.empty() && text_.back() == '\n') {
        text_.pop_back();
    }

    file.close();
}

void Subject::replaceWord(const std::string& target, const std::string& replacement) {
    if (target.empty()) { return; }

    size_t pos = 0;
    while ((pos = text_.find(target, pos)) != std::string::npos) {
        text_.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }

    saveToFile();
}

bool Subject::findPhrase(const std::string& phrase) {
    if (phrase.empty()) { return false; }
    return text_.find(phrase) != std::string::npos;
}

void Subject::capitalizeWords() {
    bool isNewWord = true;
    for (char& chr : text_) {
        if (std::isalpha(static_cast<unsigned char>(chr)) != 0) {
            if (isNewWord) {
                chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
                isNewWord = false;
            }
        }
        else if (std::isspace(static_cast<unsigned char>(chr)) != 0 || std::ispunct(static_cast<unsigned char>(chr)) != 0 ) {
            isNewWord = true;
        }
    }

    saveToFile();
}

void Subject::capitalizeSentences() {
    bool isNewSentence = true;
    for (char& chr : text_) {
        if (std::isalpha(static_cast<unsigned char>(chr)) != 0) {
            if (isNewSentence) {
                chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
                isNewSentence = false;
            }
        }
        else if (chr == '.' || chr == '!' || chr == '?') {
            isNewSentence = true;
        }
    }
    saveToFile();
}

std::string Subject::getNumberedSentences() {
    std::string result;
    std::string currentSentence;
    int count = 1;

    for (size_t i = 0; i < text_.length(); ++i) {
        currentSentence += text_[i];

        if (text_[i] == '.' || text_[i] == '!' || text_[i] == '?') {
            size_t first = currentSentence.find_first_not_of(" \t\n\r");
            if (first != std::string::npos) {
                std::string trimmed = currentSentence.substr(first);
                result += std::to_string(count++) + ". " + trimmed + "\n";
            }
            currentSentence.clear();
        }
    }

    size_t first = currentSentence.find_first_not_of(" \t\n\r");
    if (first != std::string::npos) {
        result += std::to_string(count) + ". " + currentSentence.substr(first) + "\n";
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

bool Subject::saveToFile() {
    std::ofstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Error: can't open file" << filename_ << "\n";
        return false;
    }
    file << text_;
    file.close();
    return true;
}