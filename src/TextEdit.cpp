#include "TextEdit.h"

#include <cctype>
#include <sstream>
#include <vector>

struct TextEdit::Impl {
  std::string status_found{"found"};
  std::string status_not_found{"not found"};

  static bool is_sentence_end(char symbol) noexcept {
    return symbol == '.' || symbol == '!' || symbol == '?';
  }

  static std::string trim_left(const std::string& str) {
    std::size_t pos = 0;
    while (pos < str.size() &&
           std::isspace(static_cast<unsigned char>(str[pos])) != 0) {
      ++pos;
    }
    return str.substr(pos);
  }

  static std::vector<std::string> split_sentences(const std::string& text) {
    std::vector<std::string> res;
    std::string current;

    for (char symbol : text) {
      current += symbol;
      if (is_sentence_end(symbol)) {
        std::string trimmed = trim_left(current);
        if (!trimmed.empty()) {
          res.push_back(trimmed);
        }
        current.clear();
      }
    }

    std::string tail = trim_left(current);
    if (!tail.empty()) {
      res.push_back(tail);
    }
    return res;
  }
};

TextEdit::TextEdit() : pimpl(std::make_unique<Impl>()) {}
TextEdit::~TextEdit() = default;
TextEdit::TextEdit(TextEdit&& rhs) noexcept = default;
TextEdit& TextEdit::operator=(TextEdit&& rhs) noexcept = default;

std::string TextEdit::replace_words(const std::string& text,  // NOLINT(bugprone-easily-swappable-parameters)
                                    const std::string& old_word,
                                    const std::string& new_word) const {
  if (old_word.empty() || pimpl == nullptr) {
    return text;
  }

  std::string res = text;
  std::size_t pos = 0;
  while ((pos = res.find(old_word, pos)) != std::string::npos) {
    res.replace(pos, old_word.length(), new_word);
    pos += new_word.length();
  }
  return res;
}

std::string TextEdit::find_phrase(const std::string& text,
                                  const std::string& phrase) const {
  if (!phrase.empty() && text.find(phrase) != std::string::npos) {
    return pimpl->status_found;
  }
  return pimpl->status_not_found;
}

std::string TextEdit::capitalize_words(const std::string& text) const {
  if (pimpl == nullptr) {
    return text;
  }

  std::string res = text;
  bool new_word = true;

  for (char& symbol : res) {
    auto uchar = static_cast<unsigned char>(symbol);
    if (std::isspace(uchar) != 0 || std::ispunct(uchar) != 0) {
      new_word = true;
    } else if (new_word) {
      symbol = static_cast<char>(std::toupper(uchar));
      new_word = false;
    }
  }
  return res;
}

std::string TextEdit::capitalize_sentences(const std::string& text) const {
  if (pimpl == nullptr) {
    return text;
  }

  std::string res = text;
  bool new_sentence = true;

  for (char& symbol : res) {
    auto uchar = static_cast<unsigned char>(symbol);
    if (Impl::is_sentence_end(symbol)) {
      new_sentence = true;
    } else if (new_sentence && std::isalpha(uchar) != 0) {
      symbol = static_cast<char>(std::toupper(uchar));
      new_sentence = false;
    }
  }
  return res;
}

std::string TextEdit::number_sentences(const std::string& text) const {
  if (pimpl == nullptr) {
    return text;
  }

  std::vector<std::string> sentences = Impl::split_sentences(text);
  std::ostringstream out;

  for (std::size_t idx = 0; idx < sentences.size(); ++idx) {
    if (idx > 0) {
      out << "\n";
    }
    out << (idx + 1) << ". " << sentences[idx];
  }
  return out.str();
}