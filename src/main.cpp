#include <iostream>
#include "placeholder.h"
#include "engine.h"
#include "Document.hpp"

int main() {
    try {
        std::cout << "Hello Wrapper!\n";
        std::cout << "Hello Engine!\n";
        InitialArchitecturePlaceholder::projectInitialized();

        Document doc("hello world. hello cpp! how are you?");

        auto replaceWrapper = make_wrapper(
            &doc,
            &Document::replace_words,
            {
                { "from", std::string("hello") },
                { "to",   std::string("hi") },
            });

        auto findWrapper = make_wrapper(
            &doc,
            &Document::find_phrase,
            { { "phrase", std::string("hello") } });

        auto capWordsWrapper = make_wrapper(
            &doc,
            &Document::capitalize_words,
            {});

        auto capSentWrapper = make_wrapper(
            &doc,
            &Document::capitalize_sentences,
            {});

        auto numberWrapper = make_wrapper(
            &doc,
            &Document::number_sentences,
            {});


        Engine engine;
        engine.register_command(replaceWrapper.get(), "replace");
        engine.register_command(findWrapper.get(), "find");
        engine.register_command(capWordsWrapper.get(), "cap_words");
        engine.register_command(capSentWrapper.get(), "cap_sentences");
        engine.register_command(numberWrapper.get(), "number_sentences");


        std::cout << "Initial text:\n  " << doc.replace_words("___", "") << "\n\n";

        auto result1 = engine.execute<std::string>(
            "replace",
            { { "from", std::string("world") },{ "to", std::string("C++") } });
        std::cout << "replace(from=\"world\", to=\"C++\"):\n  " << result1 << "\n\n";

        auto result2 = engine.execute<std::string>(
            "replace",
            { { "from", std::string("cpp") } });
        std::cout << "replace(from=\"cpp\", to=default \"hi\"):\n  " << result2 << "\n\n";

        auto result3 = engine.execute<std::string>("replace");
        std::cout << "replace() with default arguments:\n  " << result3 << "\n\n";

        bool hasHello = engine.execute<bool>(
            "find",
            { { "phrase", std::string("hello") } });
        std::cout << "find(\"hello\") = " << (hasHello ? "true" : "false") << "\n";

        bool hasPython = engine.execute<bool>(
            "find",
            { { "phrase", std::string("python") } });
        std::cout << "find(\"python\") = " << (hasPython ? "true" : "false") << "\n\n";

        auto capW = engine.execute<std::string>("cap_words");
        std::cout << "cap_words():\n  " << capW << "\n\n";

        auto capS = engine.execute<std::string>("cap_sentences");
        std::cout << "cap_sentences():\n  " << capS << "\n\n";

        auto numbered = engine.execute<std::string>("number_sentences");
        std::cout << "number_sentences():\n" << numbered << "\n";

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << "\n";
        return 1;
    }
}