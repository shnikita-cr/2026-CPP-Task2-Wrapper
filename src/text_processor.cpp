#include "text_processor.h"
#include <cctype>
#include <cstddef>
#include <vector>

namespace {

constexpr unsigned char kAsciiMax = 128;
constexpr unsigned char kUtf8TwoByteMask = 0xE0;
constexpr unsigned char kUtf8TwoBytePrefix = 0xC0;
constexpr unsigned char kUtf8ThreeByteMask = 0xF0;
constexpr unsigned char kUtf8ThreeBytePrefix = 0xE0;
constexpr unsigned char kUtf8FourByteMask = 0xF8;
constexpr unsigned char kUtf8FourBytePrefix = 0xF0;
constexpr unsigned char kUtf8ContinuationMask = 0xC0;
constexpr unsigned char kUtf8ContinuationMatch = 0x80;
constexpr unsigned char kCyrillicD0 = 0xD0;
constexpr unsigned char kCyrillicD1 = 0xD1;
constexpr unsigned char kCyrillicLowerA = 0xB0;
constexpr unsigned char kCyrillicLowerP = 0xBF;
constexpr unsigned char kCyrillicLowerR = 0x80;
constexpr unsigned char kCyrillicLowerYa = 0x8F;
constexpr unsigned char kCyrillicLowerYo = 0x91;
constexpr unsigned char kCyrillicUpperYo = 0x81;
constexpr unsigned char kCaseDiff20 = 0x20;

bool is_utf8_continuation(unsigned char byte_val) noexcept {
    return (byte_val & kUtf8ContinuationMask) == kUtf8ContinuationMatch;
}

std::size_t utf8_char_len(unsigned char byte_val) noexcept {
    if (byte_val < kAsciiMax) {
        return 1;
    }
    if ((byte_val & kUtf8TwoByteMask) == kUtf8TwoBytePrefix) {
        return 2;
    }
    if ((byte_val & kUtf8ThreeByteMask) == kUtf8ThreeBytePrefix) {
        return 3;
    }
    if ((byte_val & kUtf8FourByteMask) == kUtf8FourBytePrefix) {
        return 4;
    }
    return 1;
}

bool is_letter(const std::string& str, std::size_t pos) noexcept {
    if (pos >= str.size()) {
        return false;
    }
    auto byte_val = static_cast<unsigned char>(str[pos]);
    if (byte_val < kAsciiMax) {
        return std::isalpha(byte_val) != 0;
    }
    return (byte_val == kCyrillicD0 || byte_val == kCyrillicD1) && (pos + 1 < str.size());
}

bool is_word_char(const std::string& str, std::size_t pos) noexcept {
    if (pos >= str.size()) {
        return false;
    }
    auto byte_val = static_cast<unsigned char>(str[pos]);
    if (byte_val < kAsciiMax) {
        return (std::isalnum(byte_val) != 0) || (byte_val == '_');
    }
    return (byte_val == kCyrillicD0 || byte_val == kCyrillicD1) && (pos + 1 < str.size());
}

std::size_t to_upper_letter(std::string& str, std::size_t pos) noexcept {
    if (pos >= str.size()) {
        return 0;
    }
    auto byte_one = static_cast<unsigned char>(str[pos]);
    if (byte_one < kAsciiMax) {
        str[pos] = static_cast<char>(std::toupper(byte_one));
        return 1;
    }
    if (pos + 1 < str.size()) {
        auto byte_two = static_cast<unsigned char>(str[pos + 1]);
        if (byte_one == kCyrillicD0 && byte_two >= kCyrillicLowerA && byte_two <= kCyrillicLowerP) {
            str[pos + 1] = static_cast<char>(byte_two - kCaseDiff20);
            return 2;
        }
        if (byte_one == kCyrillicD1 && byte_two >= kCyrillicLowerR && byte_two <= kCyrillicLowerYa) {
            str[pos] = static_cast<char>(kCyrillicD0);
            str[pos + 1] = static_cast<char>(byte_two + kCaseDiff20);
            return 2;
        }
        if (byte_one == kCyrillicD1 && byte_two == kCyrillicLowerYo) {
            str[pos] = static_cast<char>(kCyrillicD0);
            str[pos + 1] = static_cast<char>(kCyrillicUpperYo);
            return 2;
        }
        return 2;
    }
    return 1;
}

std::size_t find_char_start_before(const std::string& str, std::size_t pos) noexcept {
    std::size_t prev = pos - 1;
    while (prev > 0 && is_utf8_continuation(static_cast<unsigned char>(str[prev]))) {
        --prev;
    }
    return prev;
}

bool check_boundary_before(
    const std::string& text, const std::string& old_word, std::size_t match_pos) noexcept {
    if (match_pos == 0) {
        return true;
    }
    std::size_t prev_char_pos = find_char_start_before(text, match_pos);
    return !(is_word_char(text, prev_char_pos) && is_word_char(old_word, 0));
}

bool check_boundary_after(
    const std::string& text, const std::string& old_word, std::size_t match_pos) noexcept {
    std::size_t next_pos = match_pos + old_word.size();
    if (next_pos >= text.size()) {
        return true;
    }
    std::size_t last_old_char_pos = find_char_start_before(old_word, old_word.size());
    return !(is_word_char(old_word, last_old_char_pos) && is_word_char(text, next_pos));
}

std::string trim(const std::string& str_input) {
    std::size_t first = str_input.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    std::size_t last = str_input.find_last_not_of(" \t\r\n");
    return str_input.substr(first, last - first + 1);
}

std::vector<std::string> extract_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::size_t start = 0;
    std::size_t char_index = 0;
    while (char_index < text.size()) {
        char character = text[char_index];
        if (character == '.' || character == '!' || character == '?') {
            while (char_index + 1 < text.size()) {
                char next_char = text[char_index + 1];
                if (next_char != '.' && next_char != '!' && next_char != '?') {
                    break;
                }
                ++char_index;
            }
            std::string sentence = trim(text.substr(start, (char_index + 1) - start));
            if (!sentence.empty()) {
                sentences.push_back(sentence);
            }
            start = char_index + 1;
        }
        ++char_index;
    }
    if (start < text.size()) {
        std::string sentence = trim(text.substr(start));
        if (!sentence.empty()) {
            sentences.push_back(sentence);
        }
    }
    return sentences;
}

} // namespace

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string TextProcessor::replace_words(
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    const std::string& text, const std::string& old_word,
    const std::string& new_word) const { // NOLINT(bugprone-easily-swappable-parameters)
    ++processed_count_;
    if (old_word.empty()) {
        return text;
    }
    std::string result;
    std::size_t pos = 0;
    while (pos < text.size()) {
        std::size_t match_pos = text.find(old_word, pos);
        if (match_pos == std::string::npos) {
            result.append(text, pos, text.size() - pos);
            break;
        }
        if (check_boundary_before(text, old_word, match_pos) &&
            check_boundary_after(text, old_word, match_pos)) {
            result.append(text, pos, match_pos - pos);
            result.append(new_word);
            pos = match_pos + old_word.size();
        } else {
            result.append(text, pos, match_pos - pos + 1);
            pos = match_pos + 1;
        }
    }
    return result;
}
// NOLINTEND(bugprone-easily-swappable-parameters)

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
std::string TextProcessor::find_phrase(
    const std::string& text, const std::string& phrase) const {
    ++processed_count_;
    if (phrase.empty()) {
        return "не найдено";
    }
    if (text.find(phrase) != std::string::npos) {
        return "найдено";
    }
    return "не найдено";
}
// NOLINTEND(bugprone-easily-swappable-parameters)

std::string TextProcessor::capitalize_words(const std::string& text) const {
    ++processed_count_;
    std::string result = text;
    bool in_word = false;
    std::size_t char_index = 0;
    while (char_index < result.size()) {
        if (is_letter(result, char_index)) {
            if (!in_word) {
                char_index += to_upper_letter(result, char_index);
                in_word = true;
            } else {
                char_index += utf8_char_len(static_cast<unsigned char>(result[char_index]));
            }
        } else {
            in_word = is_word_char(result, char_index);
            char_index += utf8_char_len(static_cast<unsigned char>(result[char_index]));
        }
    }
    return result;
}

std::string TextProcessor::capitalize_sentences(const std::string& text) const {
    ++processed_count_;
    std::string result = text;
    bool new_sentence = true;
    std::size_t char_index = 0;
    while (char_index < result.size()) {
        char character = result[char_index];
        if (new_sentence && is_letter(result, char_index)) {
            char_index += to_upper_letter(result, char_index);
            new_sentence = false;
        } else {
            if (character == '.' || character == '!' || character == '?') {
                new_sentence = true;
            }
            char_index += utf8_char_len(static_cast<unsigned char>(result[char_index]));
        }
    }
    return result;
}

std::string TextProcessor::number_sentences(const std::string& text) const {
    ++processed_count_;
    std::vector<std::string> sentences = extract_sentences(text);
    std::string result;
    for (std::size_t idx = 0; idx < sentences.size(); ++idx) {
        if (idx > 0) {
            result += "\n";
        }
        result += std::to_string(idx + 1) + ". " + sentences[idx];
    }
    return result;
}
