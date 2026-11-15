#include "document.hpp"
#include "engine.hpp"
#include "placeholder.h"
#include "wrapper.hpp"

#include <iostream>
#include <string>
#include <utility>

namespace {

// Произвольный класс, метод которого инкапсулируется в Wrapper.
class Subject {
public:
    explicit Subject(std::string tag) : tag_(std::move(tag)) {}

    std::string func(int arg1, const std::string& arg2) {
        return tag_ + ":" + std::to_string(arg1) + ":" + arg2;
    }

private:
    std::string tag_;
};

void print_heading(const std::string& heading) {
    std::cout << "=== " << heading << " ===\n";
}

void demonstrate_command_call() {
    print_heading("Вызов метода через Engine");

    constexpr int partial_argument = 7;

    Subject subject("demo");
    Wrapper wrapper(&subject, &Subject::func, {{"arg1", 0}, {"arg2", "a"}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");

    std::cout << engine.execute("command_name", {{"arg1", 4}, {"arg2", "b"}}) << '\n';
    std::cout << engine.execute("command_name", {{"arg1", partial_argument}}) << '\n';
}

void demonstrate_document() {
    print_heading("Операции над текстовым документом");

    Document document("the cat sat on the mat. the dog barked loudly!");

    Wrapper replace(&document, &Document::replace_words, {{"from", "cat"}, {"replacement", "dog"}});
    Wrapper find(&document, &Document::find_phrases, {{"phrase", "the dog"}});
    Wrapper words(&document, &Document::capitalize_words, {});
    Wrapper sentences(&document, &Document::capitalize_sentences, {});
    Wrapper numbering(&document, &Document::number_sentences, {});

    Engine engine;
    engine.register_command(&replace, "replace_words");
    engine.register_command(&find, "find_phrases");
    engine.register_command(&words, "capitalize_words");
    engine.register_command(&sentences, "capitalize_sentences");
    engine.register_command(&numbering, "number_sentences");

    std::cout << "replace_words: " << engine.execute("replace_words") << '\n';
    std::cout << "find_phrases: " << engine.execute("find_phrases") << '\n';
    std::cout << "capitalize_words: " << engine.execute("capitalize_words") << '\n';
    std::cout << "capitalize_sentences: " << engine.execute("capitalize_sentences") << '\n';
    std::cout << "number_sentences:\n" << engine.execute("number_sentences") << '\n';
}

void run_demo() {
    InitialArchitecturePlaceholder::projectInitialized();
    demonstrate_command_call();
    demonstrate_document();
}

}  // namespace

int main() {
    try {
        run_demo();
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
