#include "utf8.h"
#include <algorithm>
#include <array>
#include <cstddef>

namespace utf8 {
namespace {

constexpr char32_t kReplacement = 0xFFFDU;
constexpr char32_t kLastCodePoint = 0x10FFFFU;

// заголовки и маски байтов utf-8
constexpr unsigned kAsciiLimit = 0x80U;
constexpr unsigned kContinuationMask = 0xC0U;
constexpr unsigned kContinuationTag = 0x80U;
constexpr unsigned kContinuationBits = 0x3FU;
constexpr unsigned kBitsPerContinuation = 6U;
constexpr unsigned kTwoByteMask = 0xE0U;
constexpr unsigned kTwoByteTag = 0xC0U;
constexpr unsigned kTwoByteBits = 0x1FU;
constexpr unsigned kThreeByteMask = 0xF0U;
constexpr unsigned kThreeByteTag = 0xE0U;
constexpr unsigned kThreeByteBits = 0x0FU;
constexpr unsigned kFourByteMask = 0xF8U;
constexpr unsigned kFourByteTag = 0xF0U;
constexpr unsigned kFourByteBits = 0x07U;
constexpr char32_t kTwoByteLimit = 0x800U;
constexpr char32_t kThreeByteLimit = 0x10000U;

// регистр соотв букв
constexpr char32_t kAsciiCaseShift = 0x20U;
constexpr char32_t kLatin1LowerFirst = 0xE0U;
constexpr char32_t kLatin1LowerLast = 0xFEU;
constexpr char32_t kLatin1UpperFirst = 0xC0U;
constexpr char32_t kLatin1UpperLast = 0xDEU;
constexpr char32_t kLatin1Multiply = 0xD7U;
constexpr char32_t kLatin1Divide = 0xF7U;
constexpr char32_t kCyrillicLowerFirst = 0x430U;
constexpr char32_t kCyrillicLowerLast = 0x44FU;
constexpr char32_t kCyrillicUpperFirst = 0x410U;
constexpr char32_t kCyrillicUpperLast = 0x42FU;
constexpr char32_t kCyrillicExtraLowerFirst = 0x450U;
constexpr char32_t kCyrillicExtraLowerLast = 0x45FU;
constexpr char32_t kCyrillicExtraUpperFirst = 0x400U;
constexpr char32_t kCyrillicExtraUpperLast = 0x40FU;
constexpr char32_t kCyrillicExtraShift = 0x50U;

struct Range {
    char32_t first;
    char32_t last;
};

constexpr std::array<Range, 6> kLetterRanges{{
    {U'A', U'Z'},
    {U'a', U'z'},
    {0xC0U, 0x24FU},
    {0x370U, 0x3FFU},
    {0x400U, 0x481U},
    {0x48AU, 0x52FU},
}};

constexpr bool in_range(char32_t symbol, char32_t first, char32_t last) {
    return symbol >= first && symbol <= last;
}

struct Lead {
    std::size_t length;
    char32_t payload;
};

Lead classify(unsigned lead) {
    if (lead < kAsciiLimit) {
        return {1, lead};
    }
    if ((lead & kTwoByteMask) == kTwoByteTag) {
        return {2, lead & kTwoByteBits};
    }
    if ((lead & kThreeByteMask) == kThreeByteTag) {
        return {3, lead & kThreeByteBits};
    }
    if ((lead & kFourByteMask) == kFourByteTag) {
        return {4, lead & kFourByteBits};
    }
    return {0, 0};
}

// чтение одной кодовой точки, начиная с pos, сдвиг на длину последовательности
char32_t read_code_point(std::string_view bytes, std::size_t& pos) {
    const Lead lead = classify(static_cast<unsigned char>(bytes[pos]));
    if (lead.length == 0 || pos + lead.length > bytes.size()) {
        ++pos;
        return kReplacement;
    }
    char32_t code_point = lead.payload;
    for (std::size_t offset = 1; offset < lead.length; ++offset) {
        const unsigned next = static_cast<unsigned char>(bytes[pos + offset]);
        if ((next & kContinuationMask) != kContinuationTag) {
            ++pos;
            return kReplacement;
        }
        code_point = (code_point << kBitsPerContinuation) | (next & kContinuationBits);
    }
    pos += lead.length;
    return code_point > kLastCodePoint ? kReplacement : code_point;
}

void append_code_point(std::string& output, char32_t symbol) {
    const auto continuation = [](char32_t value, unsigned shift) {
        return static_cast<char>(kContinuationTag | ((value >> shift) & kContinuationBits));
    };
    if (symbol < kAsciiLimit) {
        output.push_back(static_cast<char>(symbol));
    } else if (symbol < kTwoByteLimit) {
        output.push_back(static_cast<char>(kTwoByteTag | (symbol >> kBitsPerContinuation)));
        output.push_back(continuation(symbol, 0));
    } else if (symbol < kThreeByteLimit) {
        output.push_back(static_cast<char>(kThreeByteTag | (symbol >> (2 * kBitsPerContinuation))));
        output.push_back(continuation(symbol, kBitsPerContinuation));
        output.push_back(continuation(symbol, 0));
    } else {
        output.push_back(static_cast<char>(kFourByteTag | (symbol >> (3 * kBitsPerContinuation))));
        output.push_back(continuation(symbol, 2 * kBitsPerContinuation));
        output.push_back(continuation(symbol, kBitsPerContinuation));
        output.push_back(continuation(symbol, 0));
    }
}

}

std::u32string decode(std::string_view bytes) {
    std::u32string result;
    result.reserve(bytes.size());
    std::size_t pos = 0;
    while (pos < bytes.size()) {
        result.push_back(read_code_point(bytes, pos));
    }
    return result;
}

std::string encode(std::u32string_view text) {
    std::string result;
    result.reserve(text.size());
    for (const char32_t symbol : text) {
        append_code_point(result, symbol);
    }
    return result;
}

char32_t to_upper(char32_t symbol) {
    if (in_range(symbol, U'a', U'z')) {
        return symbol - kAsciiCaseShift;
    }
    if (in_range(symbol, kLatin1LowerFirst, kLatin1LowerLast) && symbol != kLatin1Divide) {
        return symbol - kAsciiCaseShift;
    }
    if (in_range(symbol, kCyrillicLowerFirst, kCyrillicLowerLast)) {
        return symbol - kAsciiCaseShift;
    }
    if (in_range(symbol, kCyrillicExtraLowerFirst, kCyrillicExtraLowerLast)) {
        return symbol - kCyrillicExtraShift;
    }
    return symbol;
}

char32_t to_lower(char32_t symbol) {
    if (in_range(symbol, U'A', U'Z')) {
        return symbol + kAsciiCaseShift;
    }
    if (in_range(symbol, kLatin1UpperFirst, kLatin1UpperLast) && symbol != kLatin1Multiply) {
        return symbol + kAsciiCaseShift;
    }
    if (in_range(symbol, kCyrillicUpperFirst, kCyrillicUpperLast)) {
        return symbol + kAsciiCaseShift;
    }
    if (in_range(symbol, kCyrillicExtraUpperFirst, kCyrillicExtraUpperLast)) {
        return symbol + kCyrillicExtraShift;
    }
    return symbol;
}

bool is_letter(char32_t symbol) {
    if (symbol == kLatin1Multiply || symbol == kLatin1Divide) {
        return false;
    }
    return std::any_of(kLetterRanges.begin(), kLetterRanges.end(),
                       [symbol](const Range& range) { return in_range(symbol, range.first, range.last); });
}

bool is_digit(char32_t symbol) {
    return in_range(symbol, U'0', U'9');
}

bool is_space(char32_t symbol) {
    constexpr char32_t kNoBreakSpace = 0xA0U;
    return symbol == U' ' || in_range(symbol, U'\t', U'\r') || symbol == kNoBreakSpace;
}

}