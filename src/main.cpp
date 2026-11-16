#include "Engine.h"
#include "TextDocument.h"
#include <iostream>
#include <string>


int main() {
    try {
        Engine engine;
        std::cout << "Example 1. Text \n";
        TextDocument doc("This is a test text. Hello! How is your day?");
        std::cout << "Original: \"" << doc.get_text() << "\"\n";

        Wrapper replace_w(&doc, &TextDocument::replace_words, { {"old_word", "hello"}, {"new_word", "hi"} });
        Wrapper find_w(&doc, &TextDocument::find_phrase, { {"phrase", "test"} });
        Wrapper cap_words_w(&doc, &TextDocument::capitalize_words, {});
        Wrapper cap_sents_w(&doc, &TextDocument::capitalize_sentences, {});
        Wrapper number_w(&doc, &TextDocument::number_sentences, {});

        engine.register_command(&replace_w, "replace");
        engine.register_command(&find_w, "find_phrase");
        engine.register_command(&cap_words_w, "capitalize_words");
        engine.register_command(&cap_sents_w, "capitalize_sentences");
        engine.register_command(&number_w, "number");

        std::cout << "Replace:        " << engine.execute("replace", { {"new_word", "Let's go!!!"}, {"old_word", "Hello!"} }) << '\n';
        std::cout << "Find positions: " << engine.execute("find_phrase") << '\n';
        std::cout << "Capitalize words:      " << engine.execute("capitalize_words") << '\n';
        std::cout << "Capitalize sentences:  " << engine.execute("capitalize_sentences") << '\n';
        std::cout << "Numbering:\n" << engine.execute("number") << '\n';

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}