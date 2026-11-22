#ifndef SUBJECT_H
#define SUBJECT_H

#include <string>
#include <fstream>

class Subject {
private:
    std::string text_;
    std::string filename_;

public:
    explicit Subject(std::string filename);

    void replaceWord(const std::string& target, const std::string& replacement);
    bool findPhrase(const std::string& phrase);
    void capitalizeWords();
    void capitalizeSentences();
    std::string getNumberedSentences();

    bool saveToFile();
};

#endif