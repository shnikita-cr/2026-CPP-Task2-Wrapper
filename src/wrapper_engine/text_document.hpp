#pragma once

#include <cstddef>
#include <string>

// Хранит текст и выполняет над ним операции из условия задания.
class TextDocument
{
public:
    // Получает исходное содержимое документа.
    explicit TextDocument(std::string text);

    // Даёт доступ к текущему тексту без копирования.
    [[nodiscard]] const std::string& text() const;

    // Полностью заменяет содержимое документа.
    void set_text(std::string text);

    // Заменяет только отдельные слова, а не части других слов.
    std::string replace_words(const std::string& oldWord, const std::string& newWord);

    // Возвращает количество непересекающихся вхождений фразы.
    std::string find_phrase(const std::string& phrase);

    // Изменяет регистр первых букв всех слов.
    std::string capitalize_words();

    // Изменяет регистр первых букв предложений.
    std::string capitalize_sentences();

    // Записывает каждое предложение с номером на новой строке.
    std::string number_sentences();

private:
    // Кириллические байты считаются частью слова вместе с буквами и цифрами ASCII.
    static bool is_word_character(unsigned char character);

    // Эти знаки отделяют предложения.
    static bool is_sentence_end(char character);

    // Делает одну латинскую или русскую букву заглавной.
    void uppercase_letter_at(std::size_t position);

    // Удаляет пробельные символы по краям строки.
    static std::string trim(const std::string& value);

    std::string _text; // Текущее изменяемое содержимое документа.
};
