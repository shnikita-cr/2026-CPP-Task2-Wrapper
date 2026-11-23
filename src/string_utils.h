#pragma once

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <cctype>

namespace utils {
    template<typename T>
    T from_string(const std::string &s) {
        if constexpr (std::is_same_v<T, std::string>) return s;
        else {
            std::istringstream iss(s);
            T v{};
            iss >> v;
            return v;
        }
    }

    inline std::string to_display(const std::string &s) { return s; }
    inline std::string to_display(int v) { return std::to_string(v); }
    inline std::string to_display(long v) { return std::to_string(v); }
    inline std::string to_display(long long v) { return std::to_string(v); }
    inline std::string to_display(unsigned v) { return std::to_string(v); }
    inline std::string to_display(double v) { return std::to_string(v); }
    inline std::string to_display(bool v) { return v ? "true" : "false"; }

    inline std::vector<std::string> split_sentences(const std::string &text) {
        std::vector<std::string> sentences;
        std::string current;

        for (char c: text) {
            current.push_back(c);
            if (c == '.' || c == '!' || c == '?') {
                std::size_t end = current.size();
                while (end > 0 && std::isspace(static_cast<unsigned char>(current[end - 1]))) {
                    --end;
                }
                sentences.push_back(current.substr(0, end));
                current.clear();
            }
        }

        std::size_t begin = 0;
        while (begin < current.size() && std::isspace(static_cast<unsigned char>(current[begin]))) {
            ++begin;
        }
        if (begin < current.size()) sentences.push_back(current.substr(begin));

        return sentences;
    }
}
