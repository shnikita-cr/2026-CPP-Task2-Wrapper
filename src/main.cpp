#include "Engine.hpp"
#include "TextEditor.hpp"
#include "Wrapper.hpp"

#include <exception>
#include <iostream>
#include <string>

int main() {
    try {
        TextEditor editor(
            "code quality matters. clean code helps teams scale! do you review your codebase often?");

        Wrapper replace_cmd(
            &editor,
            &TextEditor::replaceWord,
            {{"target", std::string("code")}, {"replacement", std::string("design")}});

        Wrapper search_cmd(
            &editor, &TextEditor::searchPhrase, {{"phrase", std::string("clean code")}});

        Wrapper cap_words_cmd(&editor, &TextEditor::capitalizeEachWord);
        Wrapper cap_sentences_cmd(&editor, &TextEditor::capitalizeSentences);
        Wrapper number_sentences_cmd(&editor, &TextEditor::formatNumberedSentences);

        Engine engine;
        engine.register_command(&replace_cmd, "replace_word");
        engine.register_command(&search_cmd, "search_phrase");
        engine.register_command(&cap_words_cmd, "capitalize_words");
        engine.register_command(&cap_sentences_cmd, "capitalize_sentences");
        engine.register_command(&number_sentences_cmd, "number_sentences");

        std::cout << "Original text:\n  " << editor.content() << "\n\n";

        std::cout << "[replace_word] default arguments:\n  "
                  << engine.execute("replace_word") << "\n\n";

        std::cout << "[replace_word] partial override (replacement=\"architecture\"):\n  "
                  << engine.execute("replace_word", {{"replacement", std::string("architecture")}})
                  << "\n\n";

        std::cout << "[replace_word] full override (target=\"teams\", replacement=\"engineers\"):\n  "
                  << engine.execute("replace_word",
                                    {{"target", std::string("teams")},
                                     {"replacement", std::string("engineers")}})
                  << "\n\n";

        std::cout << "[search_phrase] default (\"clean code\"): "
                  << engine.execute("search_phrase") << "\n";
        std::cout << "[search_phrase] custom (\"legacy system\"): "
                  << engine.execute("search_phrase", {{"phrase", std::string("legacy system")}})
                  << "\n\n";

        std::cout << "[capitalize_words]:\n  " << engine.execute("capitalize_words") << "\n\n";

        std::cout << "[capitalize_sentences]:\n  " << engine.execute("capitalize_sentences")
                  << "\n\n";

        std::cout << "[number_sentences]:\n" << engine.execute("number_sentences") << "\n";

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Execution error: " << ex.what() << '\n';
        return 1;
    }
}
