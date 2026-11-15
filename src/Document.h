#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <cstddef>
#include <string>
#include <vector>

class Document {
public:
    explicit Document(std::string text = "");
    ~Document() noexcept = default;

    Document(const Document&) = default;
    Document& operator=(const Document&) = default;
    Document(Document&&) noexcept = default;
    Document& operator=(Document&&) noexcept = default;

    void set_text(const std::string& text);
    [[nodiscard]] const std::string& get_text() const noexcept;

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    std::string replace_words(const std::string& old_word, const std::string& new_word);
    [[nodiscard]] std::string find_phrase(const std::string& phrase) const;
    std::string capitalize_words(const std::string& target_word);
    std::string capitalize_sentences(const std::string& delimiters);
    std::string number_sentences(const std::string& separator);

private:
    static bool is_word_char(char symbol) noexcept;
    static bool is_sentence_end(char symbol) noexcept;
    [[nodiscard]] bool is_whole_word_at(std::size_t position, const std::string& word) const noexcept;
    void capitalize_all_words();
    static std::vector<std::string> split_sentences(const std::string& raw_text);

    std::string text_;
};

#endif 
