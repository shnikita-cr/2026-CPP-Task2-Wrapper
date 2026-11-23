#include "wrapper.h"

#include <cctype>
#include <iostream>
#include <string>

class TextDocument {
public:
    explicit TextDocument(std::string text) : text_(std::move(text)) {}

    std::string replace(const Arguments& args) {
        const std::string& from = args.at("from");
        const std::string& to = args.at("to");
        if (from.empty()) {
            throw std::invalid_argument("empty word");
        }
        std::size_t pos = 0;
        while ((pos = text_.find(from, pos)) != std::string::npos) {
            const bool left = pos == 0 || !is_word(text_[pos - 1]);
            const bool right = pos + from.size() == text_.size() ||
                               !is_word(text_[pos + from.size()]);
            if (left && right) {
                text_.replace(pos, from.size(), to);
                pos += to.size();
            } else {
                pos += from.size();
            }
        }
        return text_;
    }

    std::string find(const Arguments& args) const {
        const std::string& phrase = args.at("phrase");
        return text_.find(phrase) == std::string::npos ? "not found" : "found";
    }

    std::string capitalize_words(const Arguments&) {
        bool start = true;
        for (char& ch : text_) {
            if (is_word(ch)) {
                if (start) {
                    ch = upper(ch);
                }
                start = false;
            } else {
                start = true;
            }
        }
        return text_;
    }

    std::string capitalize_sentences(const Arguments&) {
        bool start = true;
        for (char& ch : text_) {
            if (start && is_word(ch)) {
                ch = upper(ch);
                start = false;
            }
            if (ch == '.' || ch == '!' || ch == '?') {
                start = true;
            }
        }
        return text_;
    }

    std::string number_sentences(const Arguments&) {
        std::string result;
        std::size_t begin = 0;
        int number = 1;
        while (begin < text_.size()) {
            begin = text_.find_first_not_of(" \t\n\r", begin);
            if (begin == std::string::npos) {
                break;
            }
            const std::size_t end = text_.find_first_of(".!?", begin);
            const std::size_t last = end == std::string::npos ? text_.size() : end + 1;
            result += std::to_string(number++) + ". " + text_.substr(begin, last - begin) + "\n";
            begin = last;
        }
        text_ = result;
        return text_;
    }

private:
    static bool is_word(char ch) {
        return std::isalnum(static_cast<unsigned char>(ch)) != 0;
    }

    static char upper(char ch) {
        return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }

    std::string text_;
};

int main() {
    TextDocument document("hello world. this world is small! find this phrase?");
    Wrapper replace(&document, &TextDocument::replace, {{"from", "world"}, {"to", "earth"}});
    Wrapper find(&document, &TextDocument::find, {{"phrase", "this phrase"}});
    Wrapper words(&document, &TextDocument::capitalize_words, {});
    Wrapper sentences(&document, &TextDocument::capitalize_sentences, {});
    Wrapper number(&document, &TextDocument::number_sentences, {});

    Engine engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&words, "words");
    engine.register_command(&sentences, "sentences");
    engine.register_command(&number, "number");

    std::cout << engine.execute("replace", {{"to", "planet"}}) << '\n';
    std::cout << engine.execute("find") << '\n';
    std::cout << engine.execute("words") << '\n';
    std::cout << engine.execute("sentences") << '\n';
    std::cout << engine.execute("number");
}
