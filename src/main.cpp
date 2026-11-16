#include <exception>
#include <iostream>

#include "engine.h"
#include "text_processor.h"
#include "wrapper.h"

namespace {

// Пример из задания: произвольный класс с методом от двух аргументов.
class Subject {
public:
    explicit Subject(int base) : base_(base) {}
    [[nodiscard]] int func(int first, int second) const { return base_ + first + second; }  // NOLINT(bugprone-easily-swappable-parameters)

private:
    int base_;
};

constexpr int kBase = 100;
constexpr int kFirstArg = 4;
constexpr int kSecondArg = 5;
constexpr int kOnlySecondArg = 7;

void runSubjectDemo() {
    Subject subj(kBase);
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", 0}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");
    std::cout << engine.execute("command_name", {{"arg1", kFirstArg}, {"arg2", kSecondArg}}) << '\n';
    std::cout << engine.execute("command_name", {{"arg2", kOnlySecondArg}}) << '\n';
}

void runTextDemo() {
    TextProcessor processor("To be, or not to be, that is the question. Whether tis nobler in the mind to suffer the slings and arrows of outrageous fortune.");

    Wrapper replace(&processor, &TextProcessor::replaceWord, {{"from", ""}, {"to", ""}});
    Wrapper find(&processor, &TextProcessor::findPhrase, {{"phrase", ""}});
    Wrapper words(&processor, &TextProcessor::capitalizeWords, {});
    Wrapper sentences(&processor, &TextProcessor::capitalizeSentences, {});
    Wrapper numbering(&processor, &TextProcessor::numberSentences, {});

    Engine engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&words, "capitalize_words");
    engine.register_command(&sentences, "capitalize_sentences");
    engine.register_command(&numbering, "number");

    std::cout << "[replace]\n" << engine.execute("replace", {{"from", "fortune"}, {"to", "fate"}}) << "\n\n";
    std::cout << "[find 'slings and arrows'] " << engine.execute("find", {{"phrase", "slings and arrows"}}) << '\n';
    std::cout << "[find 'to sleep'] " << engine.execute("find", {{"phrase", "to sleep"}}) << "\n\n";
    std::cout << "[capitalize_sentences]\n" << engine.execute("capitalize_sentences") << "\n\n";
    std::cout << "[capitalize_words]\n" << engine.execute("capitalize_words") << "\n\n";
    std::cout << "[number]\n" << engine.execute("number") << '\n';

    TextProcessor russian("Вы думаете, это бредит малярия? Это было, было в Одессе. Приду в четыре, сказала Мария.");
    Wrapper russianWords(&russian, &TextProcessor::capitalizeWords, {});
    engine.register_command(&russianWords, "ru_words");
    std::cout << "\n[ru_words]\n" << engine.execute("ru_words") << '\n';
}

}  // namespace

int main() {
    try {
        runSubjectDemo();
        runTextDemo();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
