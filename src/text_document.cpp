#include "text_document.h"
#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>
#include "utf8.h"

namespace {

constexpr std::size_t kNotFound = std::u32string::npos;
constexpr std::u32string_view kTerminators = U".!?…";
constexpr std::u32string_view kClosers = U")]}\"'»”’";
constexpr std::u32string_view kApostrophes = U"'’";

// полуоткрытый диапазон [begin, end)
struct Span {
    std::size_t begin;
    std::size_t end;
};

bool is_alnum(char32_t symbol) {
    return utf8::is_letter(symbol) || utf8::is_digit(symbol);
}

bool is_word_char(std::u32string_view text, std::size_t index) {
    const char32_t symbol = text[index];
    if (is_alnum(symbol)) {
        return true;
    }
    const bool between_letters = index > 0 && index + 1 < text.size() && is_alnum(text[index - 1]) &&
                                 is_alnum(text[index + 1]);
    return between_letters && kApostrophes.find(symbol) != kNotFound;
}

// поиск подстроки не прилипающая к соседним словам
class WholeWordSearch {
public:
    explicit WholeWordSearch(std::u32string needle) : m_needle(std::move(needle)) {}

    [[nodiscard]] std::size_t length() const {
        return m_needle.size();
    }

    [[nodiscard]] std::size_t find_in(std::u32string_view text, std::size_t start) const {
        std::size_t pos = text.find(m_needle, start);
        while (pos != kNotFound && !has_boundaries(text, pos)) {
            pos = text.find(m_needle, pos + 1);
        }
        return pos;
    }

private:
    [[nodiscard]] bool has_boundaries(std::u32string_view text, std::size_t pos) const {
        const std::size_t end = pos + m_needle.size();
        const bool left_free = pos == 0 || !is_word_char(text, pos - 1) || !is_word_char(m_needle, 0);
        const bool right_free = end == text.size() || !is_word_char(text, end) ||
                                !is_word_char(m_needle, m_needle.size() - 1);
        return left_free && right_free;
    }

    std::u32string m_needle;
};

std::u32string replace_all(std::u32string_view text, const WholeWordSearch& search, std::u32string_view replacement) {
    std::u32string result;
    std::size_t copied = 0;
    for (std::size_t hit = search.find_in(text, 0); hit != kNotFound; hit = search.find_in(text, copied)) {
        result.append(text.substr(copied, hit - copied));
        result.append(replacement);
        copied = hit + search.length();
    }
    result.append(text.substr(copied));
    return result;
}

// подряд идущие пробелы - один пробел, пробелы по краям убираются
std::u32string collapse_spaces(std::u32string_view text) {
    std::u32string result;
    bool pending_space = false;
    for (const char32_t symbol : text) {
        if (utf8::is_space(symbol)) {
            pending_space = !result.empty();
            continue;
        }
        if (pending_space) {
            result.push_back(U' ');
            pending_space = false;
        }
        result.push_back(symbol);
    }
    return result;
}

std::u32string normalize_for_search(std::u32string text) {
    for (char32_t& symbol : text) {
        symbol = utf8::to_lower(symbol);
    }
    return collapse_spaces(text);
}

std::u32string upper_word_starts(std::u32string text) {
    for (std::size_t i = 0; i < text.size(); ++i) {
        const bool word_start = is_word_char(text, i) && (i == 0 || !is_word_char(text, i - 1));
        if (word_start) {
            text[i] = utf8::to_upper(text[i]);
        }
    }
    return text;
}

// если на index заканчивается предложение, ставит позицию сразу за его завершением, иначе kNotFound
std::size_t sentence_end(std::u32string_view text, std::size_t index) {
    if (kTerminators.find(text[index]) == kNotFound) {
        return kNotFound;
    }
    std::size_t end = index + 1;
    while (end < text.size() && (kTerminators.find(text[end]) != kNotFound || kClosers.find(text[end]) != kNotFound)) {
        ++end;
    }
    return (end == text.size() || utf8::is_space(text[end])) ? end : kNotFound;
}

void add_trimmed(std::vector<Span>& spans, std::u32string_view text, Span raw) {
    while (raw.begin < raw.end && utf8::is_space(text[raw.begin])) {
        ++raw.begin;
    }
    while (raw.end > raw.begin && utf8::is_space(text[raw.end - 1])) {
        --raw.end;
    }
    if (raw.begin < raw.end) {
        spans.push_back(raw);
    }
}

// границы предложений
std::vector<Span> sentence_spans(std::u32string_view text) {
    std::vector<Span> spans;
    std::size_t begin = 0;
    std::size_t index = 0;
    while (index < text.size()) {
        const std::size_t end = sentence_end(text, index);
        if (end == kNotFound) {
            ++index;
            continue;
        }
        add_trimmed(spans, text, {begin, end});
        begin = end;
        index = end;
    }
    add_trimmed(spans, text, {begin, text.size()});
    return spans;
}

std::u32string upper_sentence_starts(std::u32string text) {
    for (const Span& span : sentence_spans(text)) {
        for (std::size_t i = span.begin; i < span.end && !utf8::is_digit(text[i]); ++i) {
            if (utf8::is_letter(text[i])) {
                text[i] = utf8::to_upper(text[i]);
                break;
            }
        }
    }
    return text;
}

std::u32string numbered_sentences(std::u32string_view text) {
    std::u32string result;
    std::size_t number = 0;
    for (const Span& span : sentence_spans(text)) {
        if (number != 0) {
            result.push_back(U'\n');
        }
        ++number;
        result += utf8::decode(std::to_string(number));
        result += U". ";
        result += collapse_spaces(text.substr(span.begin, span.end - span.begin));
    }
    return result;
}

template <typename Transform>
const std::string& transform_text(std::string& text, Transform transform) {
    text = utf8::encode(transform(utf8::decode(text)));
    return text;
}

}

TextDocument::TextDocument(std::string text) : m_text(std::move(text)) {}

const std::string& TextDocument::text() const {
    return m_text;
}

const std::string& TextDocument::replace_words(const std::string& replaced, const std::string& replacement) {
    if (replaced.empty()) {
        throw std::invalid_argument("текст для замены не должен быть пустым");
    }
    const WholeWordSearch search(utf8::decode(replaced));
    const std::u32string new_text = utf8::decode(replacement);
    return transform_text(m_text, [&search, &new_text](const std::u32string& text) {
        return replace_all(text, search, new_text);
    });
}

bool TextDocument::find_phrase(const std::string& phrase) const {
    std::u32string needle = normalize_for_search(utf8::decode(phrase));
    if (needle.empty()) {
        return false;
    }
    const std::u32string haystack = normalize_for_search(utf8::decode(m_text));
    return WholeWordSearch(std::move(needle)).find_in(haystack, 0) != kNotFound;
}

const std::string& TextDocument::capitalize_words() {
    return transform_text(m_text, upper_word_starts);
}

const std::string& TextDocument::capitalize_sentences() {
    return transform_text(m_text, upper_sentence_starts);
}

const std::string& TextDocument::number_sentences() {
    return transform_text(m_text, numbered_sentences);
}
