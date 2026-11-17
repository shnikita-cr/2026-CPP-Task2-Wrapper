#include "wrapper_engine/text_document.hpp"

#include <cctype>
#include <stdexcept>
#include <utility>

// С этого значения начинаются байты за пределами ASCII.
static constexpr unsigned char firstNonAsciiByte = 0x80;

TextDocument::TextDocument(std::string text)
    : _text(std::move(text))
{
}

const std::string& TextDocument::text() const
{
    // Ссылка позволяет показать документ без копирования всей строки.
    return _text;
}

void TextDocument::set_text(std::string text)
{
    // Перемещение переиспользует память переданной строки, когда это возможно.
    _text = std::move(text);
}

std::string TextDocument::replace_words(const std::string& oldWord, const std::string& newWord)
{
    // Пустая строка находилась бы в каждой позиции текста.
    if (oldWord.empty())
    {
        throw std::invalid_argument("The word to replace cannot be empty.");
    }

    std::size_t position = 0;

    // После каждой проверки продолжаем поиск с уже обработанной позиции.
    while ((position = _text.find(oldWord, position)) != std::string::npos)
    {
        // Слева должно быть начало текста или граница слова.
        const bool hasLeftBoundary = position == 0 || !is_word_character(static_cast<unsigned char>(_text[position - 1]));

        // Правая граница проверяется сразу после найденной последовательности.
        const std::size_t endPosition = position + oldWord.size();
        const bool hasRightBoundary = endPosition == _text.size() || !is_word_character(static_cast<unsigned char>(_text[endPosition]));

        if (hasLeftBoundary && hasRightBoundary)
        {
            // После замены пропускаем вставленное слово, чтобы не искать внутри него.
            _text.replace(position, oldWord.size(), newWord);
            position += newWord.size();
        }
        else
        {
            // Совпадение было частью другого слова, поэтому оставляем его без изменений.
            position += oldWord.size();
        }
    }

    return _text;
}

std::string TextDocument::find_phrase(const std::string& phrase)
{
    // Пустая фраза не задаёт осмысленный объект поиска.
    if (phrase.empty())
    {
        throw std::invalid_argument("The phrase to find cannot be empty.");
    }

    std::size_t matches = 0;
    std::size_t position = 0;

    // Сдвиг на длину фразы даёт только непересекающиеся вхождения.
    while ((position = _text.find(phrase, position)) != std::string::npos)
    {
        ++matches;
        position += phrase.size();
    }

    if (matches == 0)
    {
        // Для пользователя сообщение понятнее числового нуля.
        return "Фраза не найдена.";
    }

    return "Количество найденных вхождений: " + std::to_string(matches) + ".";
}

std::string TextDocument::capitalize_words()
{
    // Начало документа одновременно считается началом первого слова.
    bool isWordBeginning = true;

    for (std::size_t position = 0; position < _text.size(); ++position)
    {
        const bool isWordByte = is_word_character(static_cast<unsigned char>(_text[position]));

        if (isWordBeginning && isWordByte)
        {
            // Функция умеет менять регистр латиницы и кириллицы.
            uppercase_letter_at(position);
        }

        // Байт продолжения UTF-8 относится к тому же слову.
        isWordBeginning = !isWordByte;
    }

    return _text;
}

std::string TextDocument::capitalize_sentences()
{
    // В начале документа и после точки, ! или ? ждём первый символ слова.
    bool isSentenceBeginning = true;

    for (std::size_t position = 0; position < _text.size(); ++position)
    {
        const char character = _text[position];

        // После любого завершающего знака ожидаем новое предложение.
        if (is_sentence_end(character))
        {
            isSentenceBeginning = true;
        }
        else if (isSentenceBeginning && is_word_character(static_cast<unsigned char>(character)))
        {
            // Остальные байты этого символа уже не считаются началом предложения.
            uppercase_letter_at(position);
            isSentenceBeginning = false;
        }
    }

    return _text;
}

std::string TextDocument::number_sentences()
{
    std::string result;
    std::size_t sentenceNumber = 1;
    std::size_t sentenceBeginning = 0;

    while (sentenceBeginning < _text.size())
    {
        // Доходим до первого завершающего знака или до конца текста.
        std::size_t sentenceEnd = sentenceBeginning;
        while (sentenceEnd < _text.size() && !is_sentence_end(_text[sentenceEnd]))
        {
            ++sentenceEnd;
        }

        // Многоточие и сочетания ?! или !!! включаем в то же предложение.
        while (sentenceEnd < _text.size() && is_sentence_end(_text[sentenceEnd]))
        {
            ++sentenceEnd;
        }

        const std::string sentence = trim(_text.substr(sentenceBeginning, sentenceEnd - sentenceBeginning));
        if (!sentence.empty())
        {
            if (!result.empty())
            {
                result += '\n';
            }

            result += std::to_string(sentenceNumber) + ". " + sentence;
            ++sentenceNumber;
        }

        sentenceBeginning = sentenceEnd;
    }

    // Нумерация становится новым состоянием документа.
    _text = std::move(result);
    return _text;
}

bool TextDocument::is_word_character(unsigned char character)
{
    // Байты UTF-8 не должны ошибочно создавать границу внутри русской буквы.
    return character >= firstNonAsciiByte || std::isalnum(character) != 0 || character == '_';
}

bool TextDocument::is_sentence_end(char character)
{
    return character == '.' || character == '!' || character == '?';
}

void TextDocument::uppercase_letter_at(std::size_t position)
{
    const auto firstByte = static_cast<unsigned char>(_text[position]);

    // Для ASCII достаточно стандартной функции toupper().
    if (firstByte < firstNonAsciiByte)
    {
        _text[position] = static_cast<char>(std::toupper(firstByte));
        return;
    }

    // Неполную последовательность в конце строки оставляем без изменений.
    if (position + 1 >= _text.size())
    {
        return;
    }

    const auto secondByte = static_cast<unsigned char>(_text[position + 1]);

    // Первый байт отличает две группы русских строчных букв в UTF-8.
    constexpr unsigned char firstCyrillicLeadByte = 0xD0;
    constexpr unsigned char secondCyrillicLeadByte = 0xD1;
    constexpr unsigned char lowercaseAToPBegin = 0xB0;
    constexpr unsigned char lowercaseAToPEnd = 0xBF;
    constexpr unsigned char lowercaseRToYaBegin = 0x80;
    constexpr unsigned char lowercaseRToYaEnd = 0x8F;
    constexpr unsigned char caseOffset = 0x20;
    constexpr unsigned char lowercaseYo = 0x91;
    constexpr unsigned char uppercaseYo = 0x81;

    // Русские буквы а-п отличаются от А-П вторым байтом.
    if (firstByte == firstCyrillicLeadByte && secondByte >= lowercaseAToPBegin && secondByte <= lowercaseAToPEnd)
    {
        _text[position + 1] = static_cast<char>(secondByte - caseOffset);
        return;
    }

    // Для диапазона р-я меняются оба байта UTF-8.
    if (firstByte == secondCyrillicLeadByte && secondByte >= lowercaseRToYaBegin && secondByte <= lowercaseRToYaEnd)
    {
        _text[position] = static_cast<char>(firstCyrillicLeadByte);
        _text[position + 1] = static_cast<char>(secondByte + caseOffset);
        return;
    }

    // Буква ё находится вне непрерывных диапазонов а-я.
    if (firstByte == secondCyrillicLeadByte && secondByte == lowercaseYo)
    {
        _text[position] = static_cast<char>(firstCyrillicLeadByte);
        _text[position + 1] = static_cast<char>(uppercaseYo);
    }
}

std::string TextDocument::trim(const std::string& value)
{
    // Находим первый символ, который не является пробельным.
    std::size_t beginning = 0;

    while (beginning < value.size() && std::isspace(static_cast<unsigned char>(value[beginning])) != 0)
    {
        ++beginning;
    }

    // Затем двигаемся с конца до последнего непробельного символа.
    std::size_t ending = value.size();

    while (ending > beginning && std::isspace(static_cast<unsigned char>(value[ending - 1])) != 0)
    {
        --ending;
    }

    // Возвращаем только полезную часть исходной строки.
    return value.substr(beginning, ending - beginning);
}
