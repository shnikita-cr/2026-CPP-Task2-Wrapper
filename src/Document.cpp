#include "Document.h"

#include <stdexcept>
#include <utility>
#include <vector>

namespace {
// Маски UTF-8 и диапазоны допустимых кодовых точек Unicode.
constexpr unsigned char kAsciiLimit = 0x80U;
constexpr unsigned char kTwoByteMask = 0xE0U;
constexpr unsigned char kTwoBytePrefix = 0xC0U;
constexpr unsigned char kThreeByteMask = 0xF0U;
constexpr unsigned char kThreeBytePrefix = 0xE0U;
constexpr unsigned char kFourByteMask = 0xF8U;
constexpr unsigned char kFourBytePrefix = 0xF0U;
constexpr unsigned char kLeadingByteMask = 0x7FU;
constexpr unsigned char kContinuationMask = 0xC0U;
constexpr unsigned char kContinuationPrefix = 0x80U;
constexpr unsigned char kDataMask = 0x3FU;
constexpr unsigned int kDataShift = 6U;
constexpr unsigned int kMiddleShift = 12U;
constexpr unsigned int kHighShift = 18U;
constexpr char32_t kTwoByteMinimum = 0x80U;
constexpr char32_t kThreeByteMinimum = 0x800U;
constexpr char32_t kFourByteMinimum = 0x10000U;
constexpr char32_t kMaxCodepoint = 0x10FFFFU;
constexpr char32_t kSurrogateStart = 0xD800U;
constexpr char32_t kSurrogateEnd = 0xDFFFU;

// Преобразование UTF-8 в Unicode без устаревшего std::wstring_convert.
std::size_t continuation_count(unsigned char first) {
    if (first < kAsciiLimit) {
        return 0;
    }
    if ((first & kTwoByteMask) == kTwoBytePrefix) {
        return 1;
    }
    if ((first & kThreeByteMask) == kThreeBytePrefix) {
        return 2;
    }
    if ((first & kFourByteMask) == kFourBytePrefix) {
        return 3;
    }
    throw std::invalid_argument("Invalid UTF-8 sequence");
}

bool valid_codepoint(char32_t symbol) {
    return symbol <= kMaxCodepoint &&
           (symbol < kSurrogateStart || symbol > kSurrogateEnd);
}

char32_t decode_character(const std::string& text, std::size_t& pos) {
    const auto first = static_cast<unsigned char>(text[pos++]);
    const std::size_t remaining = continuation_count(first);
    if (remaining == 0) {
        return first;
    }
    if (remaining > text.size() - pos) {
        throw std::invalid_argument("Incomplete UTF-8 sequence");
    }

    char32_t symbol = first & (kLeadingByteMask >> (remaining + 1U));
    for (std::size_t count = 0; count < remaining; ++count) {
        const auto next = static_cast<unsigned char>(text[pos++]);
        if ((next & kContinuationMask) != kContinuationPrefix) {
            throw std::invalid_argument("Invalid UTF-8 continuation byte");
        }
        symbol = (symbol << kDataShift) | (next & kDataMask);
    }

    // Не допускаем сокращённых записей и некорректных значений Unicode.
    char32_t minimum = kTwoByteMinimum;
    if (remaining == 2) {
        minimum = kThreeByteMinimum;
    } else if (remaining == 3) {
        minimum = kFourByteMinimum;
    }
    if (symbol < minimum || !valid_codepoint(symbol)) {
        throw std::invalid_argument("Invalid UTF-8 codepoint");
    }
    return symbol;
}

std::u32string decode(const std::string& text) {
    std::u32string result;
    for (std::size_t pos = 0; pos < text.size();) {
        result += decode_character(text, pos);
    }
    return result;
}

void append_utf8(std::string& result, char32_t symbol) {
    if (!valid_codepoint(symbol)) {
        throw std::invalid_argument("Invalid Unicode codepoint");
    }
    if (symbol < kAsciiLimit) {
        result += static_cast<char>(symbol);
    } else if (symbol < kThreeByteMinimum) {
        result += static_cast<char>(kTwoBytePrefix | (symbol >> kDataShift));
        result += static_cast<char>(kContinuationPrefix | (symbol & kDataMask));
    } else if (symbol < kFourByteMinimum) {
        result += static_cast<char>(kThreeBytePrefix | (symbol >> kMiddleShift));
        result += static_cast<char>(kContinuationPrefix | ((symbol >> kDataShift) & kDataMask));
        result += static_cast<char>(kContinuationPrefix | (symbol & kDataMask));
    } else {
        result += static_cast<char>(kFourBytePrefix | (symbol >> kHighShift));
        result += static_cast<char>(kContinuationPrefix | ((symbol >> kMiddleShift) & kDataMask));
        result += static_cast<char>(kContinuationPrefix | ((symbol >> kDataShift) & kDataMask));
        result += static_cast<char>(kContinuationPrefix | (symbol & kDataMask));
    }
}

std::string encode(const std::u32string& text) {
    std::string result;
    for (char32_t symbol : text) {
        append_utf8(result, symbol);
    }
    return result;
}

bool is_letter(char32_t symbol) {
    return (symbol >= U'a' && symbol <= U'z') ||
           (symbol >= U'A' && symbol <= U'Z') ||
           (symbol >= U'а' && symbol <= U'я') ||
           (symbol >= U'А' && symbol <= U'Я') ||
           symbol == U'ё' || symbol == U'Ё';
}

bool is_word_char(char32_t symbol) {
    return is_letter(symbol) || (symbol >= U'0' && symbol <= U'9') ||
           symbol == U'_';
}

char32_t upper(char32_t symbol) {
    if (symbol >= U'a' && symbol <= U'z') {
        return symbol - U'a' + U'A';
    }
    if (symbol >= U'а' && symbol <= U'я') {
        return symbol - U'а' + U'А';
    }
    if (symbol == U'ё') {
        return U'Ё';
    }
    return symbol;
}

bool is_space(char32_t symbol) {
    return symbol == U' ' || symbol == U'\t' || symbol == U'\n' ||
           symbol == U'\r' || symbol == U'\v' || symbol == U'\f' ||
           symbol == U'\u00A0';
}

bool sentence_end(char32_t symbol) {
    return symbol == U'.' || symbol == U'!' || symbol == U'?';
}

bool closing_mark(char32_t symbol) {
    return symbol == U'"' || symbol == U'»' || symbol == U'”' ||
           symbol == U')';
}

std::u32string trim(const std::u32string& text) {
    std::size_t start = 0;
    std::size_t finish = text.size();
    while (start < finish && is_space(text[start])) {
        ++start;
    }
    while (finish > start && is_space(text[finish - 1])) {
        --finish;
    }
    return text.substr(start, finish - start);
}

const std::string& required(const Arguments& args, const std::string& key) {
    const auto found = args.find(key);
    if (found == args.end()) {
        throw std::invalid_argument("Missing argument: " + key);
    }
    return found->second;
}

// Считываем закрывающие кавычки и несколько знаков завершения подряд.
void read_sentence_end(const std::u32string& text, std::size_t& position,
                       std::u32string& current) {
    while (position + 1 < text.size() && sentence_end(text[position + 1])) {
        current += text[++position];
    }
    while (position + 1 < text.size() && closing_mark(text[position + 1])) {
        current += text[++position];
    }
}

void append_sentence(std::vector<std::u32string>& sentences,
                     const std::u32string& current) {
    const std::u32string cleaned = trim(current);
    if (!cleaned.empty()) {
        sentences.push_back(cleaned);
    }
}
}  // namespace

Document::Document(std::string text) : text_(std::move(text)) {}

const std::string& Document::text() const {
    return text_;
}

std::string Document::replace_words(const Arguments& args) {
    const std::u32string from = decode(required(args, "from"));
    const std::u32string replacement = decode(required(args, "to"));
    if (from.empty()) {
        throw std::invalid_argument("Replacement word must not be empty");
    }

    const std::u32string source = decode(text_);
    std::u32string result;
    for (std::size_t pos = 0; pos < source.size();) {
        // Сначала проверяем длину: иначе возможен выход за границы строки.
        const bool fits = from.size() <= source.size() - pos;
        const bool match = fits && source.compare(pos, from.size(), from) == 0;
        const bool left_ok = pos == 0 || !is_word_char(source[pos - 1]);
        const bool right_ok = match && (pos + from.size() == source.size() ||
                             !is_word_char(source[pos + from.size()]));
        if (match && left_ok && right_ok) {
            result += replacement;
            pos += from.size();
        } else {
            result += source[pos];
            ++pos;
        }
    }
    text_ = encode(result);
    return text_;
}

std::string Document::find_phrase(const Arguments& args) const {
    const std::u32string phrase = decode(required(args, "phrase"));
    if (phrase.empty()) {
        throw std::invalid_argument("Search phrase must not be empty");
    }
    return decode(text_).find(phrase) == std::u32string::npos ?
               "Не найдено" : "Найдено";
}

std::string Document::capitalize_words(const Arguments& /*unused*/) {
    std::u32string result = decode(text_);
    bool first_letter = true;
    for (char32_t& symbol : result) {
        if (is_word_char(symbol)) {
            if (first_letter) {
                symbol = upper(symbol);
            }
            first_letter = false;
        } else {
            first_letter = true;
        }
    }
    text_ = encode(result);
    return text_;
}

std::string Document::capitalize_sentences(const Arguments& /*unused*/) {
    std::u32string result = decode(text_);
    bool first_letter = true;
    for (char32_t& symbol : result) {
        if (first_letter && is_letter(symbol)) {
            symbol = upper(symbol);
            first_letter = false;
        } else if (is_word_char(symbol)) {
            first_letter = false;
        }
        if (sentence_end(symbol) || symbol == U'\n') {
            first_letter = true;
        }
    }
    text_ = encode(result);
    return text_;
}

std::string Document::number_sentences(const Arguments& /*unused*/) {
    const std::u32string source = decode(text_);
    std::vector<std::u32string> sentences;
    std::u32string current;

    for (std::size_t pos = 0; pos < source.size(); ++pos) {
        current += source[pos];
        if (sentence_end(source[pos])) {
            read_sentence_end(source, pos, current);
            append_sentence(sentences, current);
            current.clear();
        }
    }
    append_sentence(sentences, current);

    std::u32string result;
    for (std::size_t index = 0; index < sentences.size(); ++index) {
        if (index != 0) {
            result += U'\n';
        }
        result += decode(std::to_string(index + 1));
        result += U". ";
        result += sentences[index];
    }
    text_ = encode(result);
    return text_;
}
