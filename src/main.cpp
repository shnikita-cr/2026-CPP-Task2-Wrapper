#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include "Engine.h"
#include "TextDocument.h"
#include "Wrapper.h"

namespace {

class Subject {
public:
    Subject(std::string prefix, double factor) : prefix_(std::move(prefix)), factor_(factor) {}

    [[nodiscard]] std::string func(int arg1, const std::string& arg2) const {
        return prefix_ + std::to_string(arg1) + arg2;
    }

    [[nodiscard]] double scaled(double value) const {
        return value * factor_;
    }

private:
    std::string prefix_;
    double factor_;
};

void runSubjectDemo() {
    constexpr double kFactor = 1.5;
    Subject subj("result: ", kFactor);

    Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", "a"}});
    Wrapper scaler(&subj, &Subject::scaled, {{"value", 1.0}});

    Engine engine;
    engine.register_command(&wrapper, "command_name");
    engine.register_command(&scaler, "scale");

    std::cout << engine.execute("command_name") << '\n';
    std::cout << engine.execute("command_name", {{"arg1", 4}, {"arg2", "b"}}) << '\n';
    std::cout << engine.execute("command_name", {{"arg2", "c"}}) << '\n';
    std::cout << engine.execute("scale", {{"value", 4}}) << "\n\n";
}

void runDocumentDemo() {
    TextDocument document("the quick brown fox jumps. the lazy dog sleeps! does the fox win? nobody knows");

    Wrapper replace(&document, &TextDocument::replaceWords, {{"from", "fox"}, {"to", "cat"}});
    Wrapper find(&document, &TextDocument::findPhrase, {{"phrase", "lazy dog"}});
    Wrapper words(&document, &TextDocument::capitalizeWords, {});
    Wrapper sentences(&document, &TextDocument::capitalizeSentences, {});
    Wrapper numbering(&document, &TextDocument::numberSentences, {{"delimiter", ". "}});

    Engine engine;
    engine.register_command(&replace, "replace_words");
    engine.register_command(&find, "find_phrase");
    engine.register_command(&words, "capitalize_words");
    engine.register_command(&sentences, "capitalize_sentences");
    engine.register_command(&numbering, "number_sentences");

    std::cout << "Original:\n" << document.text() << "\n\n";
    std::cout << "Replace words:\n" << engine.execute("replace_words") << "\n\n";
    std::cout << "Replace words (new arguments):\n"
              << engine.execute("replace_words", {{"from", "cat"}, {"to", "fox"}}) << "\n\n";
    std::cout << "Find phrase: " << engine.execute("find_phrase") << '\n';
    std::cout << "Find phrase: " << engine.execute("find_phrase", {{"phrase", "red fox"}}) << "\n\n";
    std::cout << "Capitalize sentences:\n" << engine.execute("capitalize_sentences") << "\n\n";
    std::cout << "Number sentences:\n" << engine.execute("number_sentences", {{"delimiter", ") "}}) << "\n\n";
    std::cout << "Capitalize words:\n" << engine.execute("capitalize_words") << '\n';
}

} // namespace

int main() {
    try {
        runSubjectDemo();
        runDocumentDemo();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
