#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "engine.h"
#include "text_handler.h"
#include "wrapper.h"

int main() {
    TextHandler handler("hello world. this is a test! do you like it?");

    Engine engine;

    engine.register_command(
        std::make_unique<ConstWrapper<TextHandler, std::string, std::string, std::string>>(
            &handler, &TextHandler::replace_word,
            std::vector<std::string>{"arg1", "arg2"},
            std::map<std::string, std::string>{{"arg1", "world"}, { "arg2", "cpp" }}),
        "replace");

    engine.register_command(
        std::make_unique<ConstWrapper<TextHandler, bool, std::string>>(
            &handler, &TextHandler::find_phrase,
            std::vector<std::string>{"arg1"},
            std::map<std::string, std::string>{{"arg1", "test"}}),
        "find");

    engine.register_command(
        std::make_unique<ConstWrapper<TextHandler, std::string>>(
            &handler, &TextHandler::capitalize_words,
            std::vector<std::string>{}, std::map<std::string, std::string>{}),
        "cap_words");

    engine.register_command(
        std::make_unique<ConstWrapper<TextHandler, std::string>>(
            &handler, &TextHandler::capitalize_sentences,
            std::vector<std::string>{}, std::map<std::string, std::string>{}),
        "cap_sent");

    engine.register_command(
        std::make_unique<ConstWrapper<TextHandler, std::string>>(
            &handler, &TextHandler::number_sentences,
            std::vector<std::string>{}, std::map<std::string, std::string>{}),
        "number");

    std::cout << "=== Replace ===\n"
        << engine.execute("replace", { {"arg1", "hello"}, {"arg2", "hi"} })
        << "\n\n";

    std::cout << "=== Find ===\n"
        << engine.execute("find", { {"arg1", "hi"} }) << "\n\n";

    std::cout << "=== Cap words ===\n"
        << engine.execute("cap_words") << "\n\n";

    std::cout << "=== Cap sentences ===\n"
        << engine.execute("cap_sent") << "\n\n";

    std::cout << "=== Number sentences ===\n"
        << engine.execute("number") << "\n\n";

    return 0;
}