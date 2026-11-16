#include "engine.hpp"
#include "text_processor.hpp"
#include "wrapper.hpp"

#include <exception>
#include <iostream>
#include <string>

namespace {

std::string status_to_string(bool is_found) {
    return is_found ? "found" : "not found";
}

void run_demo() {
    TextProcessor processor;
    Engine engine;

    engine.register_command(
        "replace",
        Wrapper([&processor](const Wrapper::ArgMap& args) {
                    return processor.replace_words(args.at("text"),
                                                   args.at("from"),
                                                   args.at("to"));
                },
                {{"from", ""}, {"to", ""}}));

    engine.register_command(
        "find",
        Wrapper([&processor](const Wrapper::ArgMap& args) {
                    return status_to_string(processor.find_phrase(args.at("text"),
                                                                  args.at("phrase")));
                },
                {{"phrase", ""}}));

    engine.register_command(
        "cap_words",
        Wrapper([&processor](const Wrapper::ArgMap& args) {
                    return processor.capitalize_words(args.at("text"));
                },
                {}));

    engine.register_command(
        "cap_sent",
        Wrapper([&processor](const Wrapper::ArgMap& args) {
                    return processor.capitalize_sentences(args.at("text"));
                },
                {}));

    engine.register_command(
        "number",
        Wrapper([&processor](const Wrapper::ArgMap& args) {
                    return processor.number_sentences(args.at("text"));
                },
                {}));

    const std::string text =
        "the quick cat runs in the park. the cat loves walking! did you see this cat?";
    const std::string sentence_text =
        "hello world. this is the second paragraph! and this is the third question?";

    std::cout << "Original text: " << text << "\n\n";

    std::cout << "[replace/full] "
              << engine.execute("replace",
                                {{"text", text}, {"from", "cat"}, {"to", "dog"}})
              << '\n';

    std::cout << "[replace/partial-defaults] "
              << engine.execute("replace",
                                {{"text", "a cat sat"}, {"from", "cat"}})
              << '\n';

    std::cout << "[find/positive] "
              << engine.execute("find",
                                {{"text", text}, {"phrase", "quick cat"}})
              << '\n';
    std::cout << "[find/negative] "
              << engine.execute("find",
                                {{"text", text}, {"phrase", "slow dog"}})
              << '\n';

    std::cout << "[capitalize_words] "
              << engine.execute("cap_words", {{"text", text}})
              << '\n';

    std::cout << "[capitalize_sentences] "
              << engine.execute("cap_sent", {{"text", sentence_text}})
              << '\n';

    std::cout << "[number_sentences]\n"
              << engine.execute("number", {{"text", sentence_text}});

    std::cout << "\nTotal operations executed: "
              << processor.operations_count() << '\n';
}

} // namespace

int main() {
    try {
        run_demo();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown error\n";
        return 1;
    }
    return 0;
}