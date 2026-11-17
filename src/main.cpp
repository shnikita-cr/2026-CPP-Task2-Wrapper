#include "Engine.h"
#include "TextDocument.h"
#include "Wrapper.h"

#include <iostream>
#include <string>

int main() {
    TextDocument document(
        "hello world. this is a test! "
        "hello again. welcome to the document?"
    );

    Wrapper replaceWrapper(
        document,
        &TextDocument::replaceWords,
        {
            {"arg0", std::string("hello")},
            {"arg1", std::string("hi")}
        }
    );

    Wrapper searchWrapper(
        document,
        &TextDocument::searchPhrase,
        {
            {"arg0", std::string("world")}
        }
    );

    Wrapper wordsWrapper(
        document,
        &TextDocument::capitalizeWords
    );

    Wrapper sentencesWrapper(
        document,
        &TextDocument::capitalizeSentences
    );

    Wrapper numberingWrapper(
        document,
        &TextDocument::numberSentences
    );

    Engine engine;

    engine.registerCommand("replace", replaceWrapper);
    engine.registerCommand("search", searchWrapper);
    engine.registerCommand("capitalize_words", wordsWrapper);
    engine.registerCommand("capitalize_sentences", sentencesWrapper);
    engine.registerCommand("number_sentences", numberingWrapper);

    try {
        std::cout << "Original text:\n";
        std::cout << document.getText() << "\n\n";

        std::cout << "Replace words:\n";
        std::cout << engine.execute("replace") << "\n\n";

        std::cout << "Search phrase:\n";
        std::cout << engine.execute("search") << "\n\n";

        std::cout << "Capitalize words:\n";
        std::cout << engine.execute("capitalize_words") << "\n\n";

        std::cout << "Capitalize sentences:\n";
        std::cout << engine.execute("capitalize_sentences") << "\n\n";

        std::cout << "Number sentences:\n";
        std::cout << engine.execute("number_sentences") << "\n";
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
