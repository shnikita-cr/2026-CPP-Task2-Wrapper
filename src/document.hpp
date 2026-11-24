#pragma once

#include "wrapper.hpp"

#include <cctype>
#include <cstddef>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace detail {

    [[nodiscard]] inline std::string trim(const std::string& input) {
        const auto first = input.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return "";
        }
        const auto last = input.find_last_not_of(" \t\r\n");
        return input.substr(first, last - first + 1);
    }

    [[nodiscard]] inline std::string escape_regex(std::string_view input) {
        constexpr std::string_view specials = R"(\.^$|()[]{}*+?)";
        std::string result;
        result.reserve(input.size());
        for (char symbol : input) {
            if (specials.find(symbol) != std::string_view::npos) {
                result.push_back('\\');
            }
            result.push_back(symbol);
        }
        return result;
    }

    [[nodiscard]] inline std::optional<std::string> arg_string(const Wrapper::Args& args, std::string_view key) {
        const auto found = args.find(key);
        if (found == args.end()) {
            return std::nullopt;
        }
        if (const auto* value = std::any_cast<std::string>(&found->second)) {
            return *value;
        }
        if (const auto* value = std::any_cast<const char*>(&found->second)) {
            return *value;
        }
        if (const auto* value = std::any_cast<int>(&found->second)) {
            return std::to_string(*value);
        }
        if (const auto* value = std::any_cast<long long>(&found->second)) {
            return std::to_string(*value);
        }
        throw std::runtime_error("unluck doc" + std::string{ key });
    }

}

class Document {
public:
    explicit Document(std::string text = {}) : text_(std::move(text)) {}

    std::string replaceWords(const Wrapper::Args& args) {
        const std::string from = detail::arg_string(args, "from").value_or("");
        const std::string to = detail::arg_string(args, "to").value_or("");
        if (from.empty()) {
            return text_;
        }
        const std::regex pattern("\\b" + detail::escape_regex(from) + "\\b");
        text_ = std::regex_replace(text_, pattern, to);
        return text_;
    }

    [[nodiscard]] std::string findPhrases(const Wrapper::Args& args) const {
        const std::string phrase = detail::arg_string(args, "phrase").value_or("");
        if (phrase.empty()) {
            return "empty";
        }
        std::ostringstream output;
        std::size_t position = 0;
        std::size_t count = 0;
        while ((position = text_.find(phrase, position)) != std::string::npos) {
            ++count;
            output << count << ": position " << position << '\n';
            position += phrase.size();
        }
        if (count == 0) {
            return "not found";
        }
        return output.str();
    }

    std::string capitalizeWords([[maybe_unused]] const Wrapper::Args& args) {
        bool new_word = true;
        for (char& symbol : text_) {
            const auto byte = static_cast<unsigned char>(symbol);
            if (std::isalpha(byte) != 0) {
                if (new_word) {
                    symbol = static_cast<char>(std::toupper(byte));
                }
                new_word = false;
            }
            else {
                new_word = true;
            }
        }
        return text_;
    }

    std::string capitalizeSentences([[maybe_unused]] const Wrapper::Args& args) {
        bool sentence_start = true;
        for (char& symbol : text_) {
            const auto byte = static_cast<unsigned char>(symbol);
            if (sentence_start && std::isalpha(byte) != 0) {
                symbol = static_cast<char>(std::toupper(byte));
                sentence_start = false;
            }
            else if (std::isalpha(byte) != 0 || std::isdigit(byte) != 0) {
                sentence_start = false;
            }
            else if (symbol == '.' || symbol == '!' || symbol == '?') {
                sentence_start = true;
            }
        }
        return text_;
    }

    std::string numberSentences([[maybe_unused]] const Wrapper::Args& args) {
        std::ostringstream output;
        std::string current;
        std::size_t index = 0;
        for (char symbol : text_) {
            current.push_back(symbol);
            if (symbol == '.' || symbol == '!' || symbol == '?') {
                const auto sentence = detail::trim(current);
                if (!sentence.empty()) {
                    ++index;
                    output << index << ". " << sentence << '\n';
                }
                current.clear();
            }
        }
        const auto tail = detail::trim(current);
        if (!tail.empty()) {
            ++index;
            output << index << ". " << tail << '\n';
        }
        text_ = output.str();
        return text_;
    }

private:
    std::string text_;
};