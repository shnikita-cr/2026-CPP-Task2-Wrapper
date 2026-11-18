#include "engine.h"
#include "text_processor.h"

#include <exception>
#include <iostream>
#include <string>

class Subject {
public:
    [[nodiscard]] std::string describe(int number, const std::string& suffix) const {
        return std::to_string(number + offset_) + suffix;
    }

private:
    int offset_ = 0;
};

int main() {
    try {
        TextProcessor processor;
        const std::string sample_text = "привет, мир! это пример текста.";
        Subject subject;

        Wrapper replace_word(
            &processor, &TextProcessor::replace_word,
            {{"text", sample_text}, {"old_word", "мир"}, {"new_word", "друг"}},
            {"text", "old_word", "new_word"});
        Wrapper find_phrase(&processor, &TextProcessor::find_phrase,
                            {{"text", sample_text}, {"phrase", "привет, мир"}},
                            {"text", "phrase"});
        Wrapper capitalize_words(&processor, &TextProcessor::capitalize_words,
                                 {{"text", sample_text}}, {"text"});
        Wrapper capitalize_sentences(&processor, &TextProcessor::capitalize_sentences,
                                      {{"text", sample_text}}, {"text"});
        Wrapper number_sentences(&processor, &TextProcessor::number_sentences,
                                 {{"text", sample_text}}, {"text"});
        Wrapper typed_example(&subject, &Subject::describe,
                              {{"number", 0}, {"suffix", std::string{"!"}}},
                              {"number", "suffix"});

        Engine engine;
        engine.register_command(&replace_word, "replace_word");
        engine.register_command(&find_phrase, "find_phrase");
        engine.register_command(&capitalize_words, "capitalize_words");
        engine.register_command(&capitalize_sentences, "capitalize_sentences");
        engine.register_command(&number_sentences, "number_sentences");
        engine.register_command(&typed_example, "typed_example");

        std::cout << "Замена слова: "
                  << engine.execute("replace_word", {{"old_word", "мир"}, {"new_word", "друг"}})
                  << '\n';
        std::cout << "Поиск фразы: "
                  << engine.execute("find_phrase", {{"phrase", "привет, мир"}}) << '\n';
        std::cout << "Заглавные буквы в словах: " << engine.execute("capitalize_words") << '\n';
        std::cout << "Заглавные буквы в предложениях: "
                  << engine.execute("capitalize_sentences") << '\n';
        std::cout << "Нумерация предложений:\n" << engine.execute("number_sentences") << '\n';
        std::cout << "Пример разных типов аргументов: "
                  << engine.execute("typed_example", {{"number", 4}}) << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка выполнения: " << error.what() << '\n';
        return 1;
    }
}
