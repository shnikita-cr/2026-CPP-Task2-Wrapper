#include "subject.h"

#include <cctype>
#include <cstddef>
#include <cstring>
#include <vector>

namespace {

bool is_alnum(char symbol) {
    return std::isalnum(static_cast<unsigned char>(symbol)) != 0;
}

bool is_space(char symbol) {
    return std::isspace(static_cast<unsigned char>(symbol)) != 0;
}

bool is_word_char(const std::string& text, std::size_t pos) {
    if (is_alnum(text[pos])) {
        return true;
    }
    return (text[pos] == '\'' || text[pos] == '-') && pos > 0 && pos + 1 < text.size() &&
           is_alnum(text[pos - 1]) && is_alnum(text[pos + 1]);
}

std::vector<std::size_t> sentence_starts(const std::string& text) {
    std::vector<std::size_t> starts;
    bool at_start = true;
    bool pending_end = false;
    for (std::size_t pos = 0; pos < text.size(); ++pos) {
        const char symbol = text[pos];
        if (is_space(symbol)) {
            if (pending_end) {
                at_start = true;
                pending_end = false;
            }
            continue;
        }
        if (at_start) {
            starts.push_back(pos);
            at_start = false;
        }
        if (symbol == '.' || symbol == '!' || symbol == '?') {
            pending_end = true;
        } else if (pending_end && std::strchr("\"')]", symbol) != nullptr) {
            continue;
        } else {
            pending_end = false;
        }
    }
    return starts;
}

}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string Subject::replace_words(const std::string& from, const std::string& replacement) {
    if (from.empty()) {
        return text_;
    }
    std::string result;
    result.reserve(text_.size());
    const std::size_t size = text_.size();
    std::size_t pos = 0;
    while (pos < size) {
        if (!is_word_char(text_, pos)) {
            result.push_back(text_[pos]);
            ++pos;
            continue;
        }
        std::size_t stop = pos;
        while (stop < size && is_word_char(text_, stop)) {
            ++stop;
        }
        if (stop - pos == from.size() && text_.compare(pos, stop - pos, from) == 0) {
            result.append(replacement);
        } else {
            result.append(text_, pos, stop - pos);
        }
        pos = stop;
    }
    text_ = std::move(result);
    return text_;
}

std::string Subject::find_phrase(const std::string& phrase) const {
    std::string result;
    if (phrase.empty()) {
        return result;
    }
    const std::size_t size = text_.size();
    std::size_t pos = text_.find(phrase);
    while (pos != std::string::npos) {
        const std::size_t end = pos + phrase.size();
        const bool left_ok = pos == 0 || !is_alnum(phrase.front()) || !is_alnum(text_[pos - 1]);
        const bool right_ok = end == size || !is_alnum(phrase.back()) || !is_alnum(text_[end]);
        if (left_ok && right_ok) {
            if (!result.empty()) {
                result += ", ";
            }
            result += std::to_string(pos);
        }
        pos = text_.find(phrase, pos + 1);
    }
    return result;
}

std::string Subject::capitalize_words() {
    const std::size_t size = text_.size();
    std::size_t pos = 0;
    while (pos < size) {
        if (!is_word_char(text_, pos)) {
            ++pos;
            continue;
        }
        text_[pos] = static_cast<char>(std::toupper(static_cast<unsigned char>(text_[pos])));
        while (pos < size && is_word_char(text_, pos)) {
            ++pos;
        }
    }
    return text_;
}

std::string Subject::capitalize_sentences() {
    for (const std::size_t start : sentence_starts(text_)) {
        std::size_t pos = start;
        while (pos < text_.size() && std::strchr("\"'([", text_[pos]) != nullptr) {
            ++pos;
        }
        if (pos < text_.size()) {
            text_[pos] = static_cast<char>(std::toupper(static_cast<unsigned char>(text_[pos])));
        }
    }
    return text_;
}

std::string Subject::number_sentences() {
    const std::vector<std::size_t> starts = sentence_starts(text_);
    std::string result;
    for (std::size_t k = 0; k < starts.size(); ++k) {
        const std::size_t begin = starts[k];
        std::size_t end = (k + 1 < starts.size()) ? starts[k + 1] : text_.size();
        while (end > begin && is_space(text_[end - 1])) {
            --end;
        }
        if (k > 0) {
            result += '\n';
        }
        result += std::to_string(k + 1) + ". " + text_.substr(begin, end - begin);
    }
    text_ = std::move(result);
    return text_;
}
