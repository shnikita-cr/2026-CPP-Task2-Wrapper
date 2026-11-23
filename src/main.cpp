#include "wrapper.h"

#include <cctype>
#include <iostream>
#include <string>

class TextDocument {
public:
  explicit TextDocument(std::string text) : text_(std::move(text)) {}

  std::string replace(const Arguments &args) {
    const std::string &from = args.at("from");
    const std::string &replacement = args.at("to");
    if (from.empty()) {
      throw std::invalid_argument("empty word");
    }
    std::size_t pos = 0;
    while ((pos = text_.find(from, pos)) != std::string::npos) {
      const bool left = pos == 0 || !is_word(text_[pos - 1]);
      const bool right = pos + from.size() == text_.size() ||
                         !is_word(text_[pos + from.size()]);
      if (left && right) {
        text_.replace(pos, from.size(), replacement);
        pos += replacement.size();
      } else {
        pos += from.size();
      }
    }
    return text_;
  }

  [[nodiscard]] std::string find(const Arguments &args) const {
    const std::string &phrase = args.at("phrase");
    return text_.find(phrase) == std::string::npos ? "not found" : "found";
  }

  std::string capitalize_words() {
    bool start = true;
    for (char &character : text_) {
      if (is_word(character)) {
        if (start) {
          character = upper(character);
        }
        start = false;
      } else {
        start = true;
      }
    }
    return text_;
  }

  std::string capitalize_sentences() {
    bool start = true;
    for (char &character : text_) {
      if (start && is_word(character)) {
        character = upper(character);
        start = false;
      }
      if (character == '.' || character == '!' || character == '?') {
        start = true;
      }
    }
    return text_;
  }

  std::string number_sentences() {
    std::string result;
    std::size_t begin = 0;
    int number = 1;
    while (begin < text_.size()) {
      begin = text_.find_first_not_of(" \t\n\r", begin);
      if (begin == std::string::npos) {
        break;
      }
      const std::size_t end = text_.find_first_of(".!?", begin);
      const std::size_t last =
          end == std::string::npos ? text_.size() : end + 1;
      result += std::to_string(number++) + ". " +
                text_.substr(begin, last - begin) + "\n";
      begin = last;
    }
    text_ = result;
    return text_;
  }

private:
  [[nodiscard]] static bool is_word(char character) {
    return std::isalnum(static_cast<unsigned char>(character)) != 0;
  }

  [[nodiscard]] static char upper(char character) {
    return static_cast<char>(
        std::toupper(static_cast<unsigned char>(character)));
  }

  std::string text_;
};

int main() {
  try {
    TextDocument document(
        "hello world. this world is small! find this phrase?");
    Wrapper replace(&document, &TextDocument::replace,
                    {{"from", "world"}, {"to", "earth"}});
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
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
