#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <string>

#include "Wrapper.h"
#include "Engine.h"
#include "TextDocument.h"

auto main() -> int {
    try {
        TextDocument document("hello world. hello cpp! this is a test.");

        Wrapper replace(&document,&TextDocument::replaceWords,{{"oldWord", std::string("hello")},{"newWord", std::string("hi")}});
        Wrapper find(&document,&TextDocument::findPhrase,{{"phrase", std::string("cpp")}});
        Wrapper capitalizeWords(&document,&TextDocument::capitalizeWords,{});
        Wrapper capitalizeSentences(&document,&TextDocument::capitalizeSentences,{});
        Wrapper numberSentences(&document,&TextDocument::numberSentences,{});

        Engine engine;
        engine.register_command(&replace, "replace");
        engine.register_command(&find, "find");
        engine.register_command(&capitalizeWords, "capitalize_words");
        engine.register_command(&capitalizeSentences, "capitalize_sentences");
        engine.register_command(&numberSentences, "number_sentences");

        std::cout << "Original text:\n" << document.getText() << "\n\n";
        std::cout << "Find phrase:\n" << engine.execute("find", {{"phrase", std::string("cpp")}}) << "\n\n";
        std::cout << "Replace words:\n" << engine.execute("replace",{{"oldWord", std::string("hello")},{"newWord", std::string("hi")}}) << "\n\n";
        std::cout << "Capitalize words:\n" << engine.execute("capitalize_words", {}) << "\n\n";
        std::cout << "Capitalize sentences:\n" << engine.execute("capitalize_sentences", {}) << "\n\n";
        std::cout << "Number sentences:\n" << engine.execute("number_sentences", {}) << '\n';

        return 0;
    }
    catch (const std::invalid_argument& e) {
        std::fprintf(stderr, "Invalid argument: %s\n", e.what());
    }
    catch (const std::out_of_range& e) {
        std::fprintf(stderr, "Out of range: %s\n", e.what());
    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "Application error: %s\n", e.what());
    }
    catch (...) {
        std::fprintf(stderr, "Unknown application error\n");
    }

    return 1;
}
