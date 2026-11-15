#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "Engine.h"
#include "TextDocument.h"
#include "Wrapper.h"

namespace {

class Subject {
public:
    explicit Subject(std::string separator) : separator_(std::move(separator)) {}

    [[nodiscard]] std::string func(int count, const std::string& word) const {
        std::string result;
        for (int i = 0; i < count; ++i) {
            if (i > 0) {
                result += separator_;
            }
            result += word;
        }
        return result;
    }

private:
    std::string separator_;
};

void run(const Engine& engine, const std::string& command, const Arguments& arguments = {}) {
    std::cout << "> " << command;
    for (const auto& argument : arguments) {
        std::cout << ' ' << argument.first << "=\"" << argument.second.text() << '"';
    }
    std::cout << '\n';
    try {
        std::cout << engine.execute(command, arguments) << "\n\n";
    } catch (const std::invalid_argument& error) {
        std::cout << "error: " << error.what() << "\n\n";
    }
}

void demo_task_example() {
    std::cout << "=== Example from the task ===\n\n";
    Subject subj("-");
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", "a"}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");

    run(engine, "command_name", {{"arg1", 4}, {"arg2", "b"}});
    run(engine, "command_name", {{"arg1", 3}});      // arg2 берётся по умолчанию
    run(engine, "command_name", {{"arg3", 1}});      // нет такого аргумента
    run(engine, "command_name", {{"arg1", "many"}});  // не число
    run(engine, "other_command");                      // нет такой команды
}

void demo_text_document() {
    std::cout << "=== Text document ===\n\n";
    TextDocument document(
        "the quick brown fox jumps over the lazy dog. the dog sleeps!  "
        "does the fox care?\nno, it does not");
    std::cout << document.text() << "\n\n";

    Wrapper replace(&document, &TextDocument::replace_word, {{"word", "fox"}, {"replacement", "cat"}});
    Wrapper find(&document, &TextDocument::find_phrase, {{"phrase", "lazy dog"}});
    Wrapper capitalize_words(&document, &TextDocument::capitalize_words, {});
    Wrapper capitalize_sentences(&document, &TextDocument::capitalize_sentences, {});
    Wrapper number_sentences(&document, &TextDocument::number_sentences, {});

    Engine engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&capitalize_words, "capitalize_words");
    engine.register_command(&capitalize_sentences, "capitalize_sentences");
    engine.register_command(&number_sentences, "number_sentences");

    run(engine, "find");
    run(engine, "find", {{"phrase", "lazy   cat"}});
    run(engine, "replace");
    run(engine, "replace", {{"replacement", "wolf"}, {"word", "dog"}});
    run(engine, "find", {{"phrase", "lazy wolf"}});
    run(engine, "capitalize_sentences");
    run(engine, "number_sentences");
    run(engine, "capitalize_words");
    run(engine, "replace", {{"word", "two words"}});
}

}  // namespace

int main() {
    try {
        demo_task_example();
        demo_text_document();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "unexpected error: " << error.what() << '\n';
    } catch (...) {
        std::cerr << "unexpected error\n";
    }
    return 1;
}
