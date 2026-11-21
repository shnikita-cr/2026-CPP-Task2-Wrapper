
#include <exception>
#include <iostream>
#include <string>
#include "TextProcessor.h"
#include "Wrapper.h"
#include "Engine.h"


int main() {

    try {
        TextProcessor text(
            "   hello world.   \n How is world? \n \tbYe world."
        );

        // Wrappers for TextProcessor methods
        Wrapper text_wrapper(&text, &TextProcessor::text,{});
        Wrapper replace_wrapper(&text, &TextProcessor::replace_words,
            {
                {"old_word", std::string("world")},
                {"new_word", std::string("program")},
            }
        );
        Wrapper find_wrapper(&text, &TextProcessor::find_phrase,
            {
                {"phrase", std::string("student")},
            }
        );
        Wrapper sentence_caps_wrapper(&text, &TextProcessor::capitalize_sentence_starts, {});
        Wrapper word_caps_wrapper(&text, &TextProcessor::capitalize_each_word, {});
        Wrapper number_wrapper(&text, &TextProcessor::number_sentences, {});

        // Creating Engine and registering commands
        Engine engine;
        engine.register_command(&text_wrapper, "get_text");
        engine.register_command(&replace_wrapper, "replace_words");
        engine.register_command(&find_wrapper, "find_phrase");
        engine.register_command(&sentence_caps_wrapper, "capitalize_sentence_starts");
        engine.register_command(&word_caps_wrapper, "capitalize_each_word");
        engine.register_command(&number_wrapper, "number_sentences");

        // Execute commands through Engine.
        std::cout << "Initial text:\n";
        std::cout << engine.execute("get_text") << "\n\n";
        std::cout << "After replacement:\n";
        std::cout << engine.execute(
            "replace_words",
            {
                {"new_word", std::string("student")},
            }
        ) << "\n\n";

        std::cout << "Phrase search:\n";
        std::cout << engine.execute("find_phrase") << "\n\n";

        std::cout << "Capitalized sentence starts:\n";
        std::cout << engine.execute("capitalize_sentence_starts") << "\n\n";

        std::cout << "Capitalized words:\n";
        std::cout << engine.execute("capitalize_each_word") << "\n\n";

        std::cout << "Numbered sentences:\n";
        std::cout << engine.execute("number_sentences") << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
