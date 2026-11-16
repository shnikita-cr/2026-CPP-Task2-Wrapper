#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <string>

// Обработчик текстового документа (поддерживает ASCII и кириллицу в UTF-8).
class TextProcessor {
public:
    explicit TextProcessor(std::string text);

    // Замена целых слов; возвращает изменённый текст.
    std::string replaceWord(const std::string& oldWord, const std::string& newWord);
    // Поиск словосочетания: "found" / "not found".
    [[nodiscard]] std::string findPhrase(const std::string& phrase) const;
    // Первая буква каждого слова - заглавная.
    std::string capitalizeWords();
    // Первая буква каждого предложения - заглавная.
    std::string capitalizeSentences();
    // Нумерация предложений, каждое с новой строки.
    std::string numberSentences();

    [[nodiscard]] const std::string& text() const { return text_; }

private:
    std::string text_;
};

#endif // TEXT_PROCESSOR_H
