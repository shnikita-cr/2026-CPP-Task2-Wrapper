#ifndef TEXT_EDIT_H
#define TEXT_EDIT_H

#include <memory>
#include <string>

class TextEdit {
 public:
  TextEdit();
  ~TextEdit();

  TextEdit(const TextEdit&) = delete;
  TextEdit& operator=(const TextEdit&) = delete;

  TextEdit(TextEdit&& rhs) noexcept;
  TextEdit& operator=(TextEdit&& rhs) noexcept;

 std::string replace_words(const std::string& text,  // NOLINT(bugprone-easily-swappable-parameters)
                            const std::string& old_word,
                            const std::string& new_word) const;

  std::string find_phrase(const std::string& text,
                          const std::string& phrase) const;

  std::string capitalize_words(const std::string& text) const;

  std::string capitalize_sentences(const std::string& text) const;

  std::string number_sentences(const std::string& text) const;

 private:
  struct Impl;
  std::unique_ptr<Impl> pimpl;
};

#endif