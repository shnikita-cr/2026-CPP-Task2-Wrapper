#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <cstddef>
#include <string>

class TextProcessor {
private:
    mutable std::size_t processed_count_ = 0;

public:
    TextProcessor() = default;

    // NOLINTBEGIN(bugprone-easily-swappable-parameters)
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    std::string replace_words(
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        const std::string& text, const std::string& old_word,
        const std::string& new_word) const; // NOLINT(bugprone-easily-swappable-parameters)
    // NOLINTEND(bugprone-easily-swappable-parameters)

    // NOLINTBEGIN(bugprone-easily-swappable-parameters)
    std::string find_phrase(const std::string& text, const std::string& phrase) const;
    // NOLINTEND(bugprone-easily-swappable-parameters)

    std::string capitalize_words(const std::string& text) const;

    std::string capitalize_sentences(const std::string& text) const;

    std::string number_sentences(const std::string& text) const;

    [[nodiscard]] std::size_t processed_count() const noexcept {
        return processed_count_;
    }
};

#endif // TEXT_PROCESSOR_H
