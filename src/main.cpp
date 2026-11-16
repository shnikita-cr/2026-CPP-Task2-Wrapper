#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include "engine.h"
#include "text_document.h"
#include "wrapper.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

// произвольный класс его метод принимает int и строку
class Subject {
public:
    explicit Subject(std::string name) : m_name(std::move(name)) {}

    [[nodiscard]] std::string func(int arg1, const std::string& arg2) const {
        return m_name + ".func(arg1=" + std::to_string(arg1) + ", arg2=" + arg2 + ")";
    }

private:
    std::string m_name;
};

void demo_architecture() {
    std::cout << "1. Wrapper + Engine \n";

    Subject subj("subj");
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", "a"}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");

    std::cout << engine.execute("command_name") << '\n';
    std::cout << engine.execute("command_name", {{"arg1", 4}}) << '\n';
    std::cout << engine.execute("command_name", {{"arg1", 4}, {"arg2", "b"}}) << "\n\n";
}

void demo_document() {
    std::cout << "2. Работа с текстовым документом \n";

    TextDocument document("the quick brown fox jumps over the lazy dog. is the dog really lazy? yes, it is!");
    std::cout << "Исходный текст:\n" << document.text() << "\n\n";

    Wrapper replace(&document, &TextDocument::replace_words, {{"replaced", "lazy"}, {"replacement", "sleepy"}});
    Wrapper find(&document, &TextDocument::find_phrase, {{"phrase", "quick brown fox"}});
    Wrapper capital_words(&document, &TextDocument::capitalize_words, {});
    Wrapper capital_sentences(&document, &TextDocument::capitalize_sentences, {});
    Wrapper numbering(&document, &TextDocument::number_sentences, {});

    Engine engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&capital_words, "capitalize_words");
    engine.register_command(&capital_sentences, "capitalize_sentences");
    engine.register_command(&numbering, "number_sentences");

    std::cout << "find \"quick brown fox\": " << engine.execute("find") << '\n';
    std::cout << "find \"slow fox\": " << engine.execute("find", {{"phrase", "slow fox"}}) << "\n\n";

    std::cout << "replace lazy -> sleepy:\n" << engine.execute("replace") << "\n\n";
    std::cout << "replace dog -> cat:\n" << engine.execute("replace", {{"replaced", "dog"}, {"replacement", "cat"}}) << "\n\n";
    std::cout << "capitalize_sentences:\n" << engine.execute("capitalize_sentences") << "\n\n";
    std::cout << "capitalize_words:\n" << engine.execute("capitalize_words") << "\n\n";
    std::cout << "number_sentences:\n" << engine.execute("number_sentences") << "\n\n";
}

void demo_russian_document() {
    std::cout << "3. UTF-8 (русский текст) \n";

    TextDocument document("привет, мир. это пример документа. работает ли он?");
    Wrapper capital_sentences(&document, &TextDocument::capitalize_sentences, {});
    Wrapper numbering(&document, &TextDocument::number_sentences, {});

    Engine engine;
    engine.register_command(&capital_sentences, "capitalize_sentences");
    engine.register_command(&numbering, "number_sentences");

    std::cout << engine.execute("capitalize_sentences") << "\n\n";
    std::cout << engine.execute("number_sentences") << "\n\n";
}

void demo_errors() {
    std::cout << "4. Обработка ошибок \n";

    Subject subj("subj");
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", "a"}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");

    try {
        std::cout << engine.execute("no_such_command") << '\n';
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << '\n';
    }
    try {
        std::cout << engine.execute("command_name", {{"arg3", 1}}) << '\n';
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << '\n';
    }
    try {
        std::cout << engine.execute("command_name", {{"arg1", "not a number"}}) << '\n';
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << '\n';
    }
}

}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        demo_architecture();
        demo_document();
        demo_russian_document();
        demo_errors();
    } catch (const std::exception& ex) {
        std::cerr << "Критическая ошибка: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}