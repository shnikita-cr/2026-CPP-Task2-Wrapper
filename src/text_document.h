#ifndef WRAPPER_TEXT_DOCUMENT_H
#define WRAPPER_TEXT_DOCUMENT_H

#include <string>

class TextDocument
{
  public:
    explicit TextDocument(std::string text);

    [[nodiscard]] const std::string& text() const noexcept;
    [[nodiscard]] std::string        replace_words(const std::string& from,
                                                   const std::string& replacement_text);
    [[nodiscard]] std::string        find_phrase(const std::string& phrase) const;
    [[nodiscard]] std::string        capitalize_words();
    [[nodiscard]] std::string        capitalize_sentences();

  private:
    [[nodiscard]] std::string update_text(const std::u32string& value);

    std::string text_;
};

#endif // WRAPPER_TEXT_DOCUMENT_H
