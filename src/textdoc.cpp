#include "textdoc.h"
#include <cctype>
#include <utility>

using namespace std;

textdoc::textdoc(string text) : text_(std::move(text)) {}

string textdoc::replaceWord(const string& target, const string& replacement) const {
    if (target.empty()) { return text_; }
    string res = text_;
    size_t pos = 0;
    while ((pos = res.find(target, pos)) != string::npos) {
        res.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }
    return res;
}

string textdoc::findPhrase(const string& phrase) const {
    if (phrase.empty()) { return "Empty phrase."; }
    size_t pos = text_.find(phrase);
    if (pos != string::npos) {
        return "Found at: " + to_string(pos);
    }
    return "Not found";
}

string textdoc::capitalizeWords() const {
    string res = text_;
    bool flag = true;
    for (char& chr : res) {
        if (isspace(static_cast<unsigned char>(chr)) != 0) {
            flag = true;
        }
        else if (flag && isalpha(static_cast<unsigned char>(chr)) != 0) {
            chr = static_cast<char>(toupper(static_cast<unsigned char>(chr)));
            flag = false;
        }
        else {
            flag = false;
        }
    }
    return res;
}

string textdoc::capitalizeSentences() const {
    string res = text_;
    bool flag = true;
    for (char& chr : res) {
        if (flag && isalpha(static_cast<unsigned char>(chr)) != 0) {
            chr = static_cast<char>(toupper(static_cast<unsigned char>(chr)));
            flag = false;
        }
        else if (chr == '.' || chr == '!' || chr == '?' || chr == '\n') {
            flag = true;
        }
    }
    return res;
}

string textdoc::numberSentences() const {
    string res;
    string current;
    int count = 1;

    for (char chr : text_) {
        current += chr;
        if (chr == '.' || chr == '!' || chr == '?') {
            size_t start = current.find_first_not_of(" \t\r\n");
            if (start != string::npos) {
                res += to_string(count++) + ". " + current.substr(start) + "\n";
            }
            current.clear();
        }
    }

    size_t start = current.find_first_not_of(" \t\r\n");
    if (start != string::npos) {
        res += to_string(count) + ". " + current.substr(start);
    }
    return res;
}

const string& textdoc::getText() const {
    return text_;
}