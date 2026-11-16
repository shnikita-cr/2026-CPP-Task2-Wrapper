#include <exception>
#include <iostream>
#include <string>

#include "engine.h"
#include "text_document.h"
#include "wrapper.h"

namespace {


class Subject {
public:
    std::string func(const std::string& arg1, const std::string& arg2) {
        ++calls_;
        return std::to_string(std::stoi(arg1) + std::stoi(arg2));
    }

    [[nodiscard]] int calls() const { return calls_; }

private:
    int calls_ = 0;
};

void print_step(const std::string& title, const std::string& result) {
    std::cout << "--- " << title << " ---\n" << result << "\n\n";
}

void run_subject_example() {
    Subject subj;
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", "0"}, {"arg2", "0"}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");

    print_step("command_name {arg1=4, arg2=5}",
               engine.execute("command_name", {{"arg1", "4"}, {"arg2", "5"}}));
    print_step("command_name {arg2=7} (arg1 is default)",
               engine.execute("command_name", {{"arg2", "7"}}));
    print_step("Subject::func calls", std::to_string(subj.calls()));
}

void run_text_example() {
    TextDocument document;

    Wrapper set_text(&document, &TextDocument::set_text, {{"text", ""}});
    Wrapper show(&document, &TextDocument::text, {});
    Wrapper replace(&document, &TextDocument::replace_word, {{"from", ""}, {"to", ""}});
    Wrapper find(&document, &TextDocument::find_phrase, {{"phrase", ""}});
    Wrapper capitalize_words(&document, &TextDocument::capitalize_words, {});
    Wrapper capitalize_sentences(&document, &TextDocument::capitalize_sentences, {});
    Wrapper number_sentences(&document, &TextDocument::number_sentences, {});

    Engine engine;
    engine.register_command(&set_text, "set_text");
    engine.register_command(&show, "show");
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&capitalize_words, "capitalize_words");
    engine.register_command(&capitalize_sentences, "capitalize_sentences");
    engine.register_command(&number_sentences, "number_sentences");

    print_step("set_text",
               engine.execute("set_text", {{"text", "the quick brown fox jumps over the lazy dog. "
                                                    "the dog didn't care! is the fox still "
                                                    "here? the catalog\nlists every cat."}}));
    print_step("find {phrase=\"lazy dog\"}", engine.execute("find", {{"phrase", "lazy dog"}}));
    print_step("find {phrase=\"lazy cat\"}", engine.execute("find", {{"phrase", "lazy cat"}}));
    print_step("replace {from=cat, to=bird}",
               engine.execute("replace", {{"from", "cat"}, {"to", "bird"}}));
    print_step("capitalize_sentences", engine.execute("capitalize_sentences"));
    print_step("number_sentences", engine.execute("number_sentences"));
    print_step("capitalize_words", engine.execute("capitalize_words"));
    print_step("show", engine.execute("show"));
}

void run_error_example() {
    Engine engine;
    try {
        static_cast<void>(engine.execute("unknown"));
    } catch (const std::exception& error) {
        print_step("error: unknown command", error.what());
    }

    TextDocument document("text");
    Wrapper find(&document, &TextDocument::find_phrase, {{"phrase", "text"}});
    engine.register_command(&find, "find");
    try {
        static_cast<void>(engine.execute("find", {{"wrong_name", "x"}}));
    } catch (const std::exception& error) {
        print_step("error: unknown argument", error.what());
    }
}

}  // namespace

int main() {
    try {
        run_subject_example();
        run_text_example();
        run_error_example();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown error\n";
        return 1;
    }
    return 0;
}
