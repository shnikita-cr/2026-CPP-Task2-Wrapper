#include "document.hpp"
#include "engine.hpp"

#include <iostream>
#include <string>

int main() {
    try{
        Document doc("happy new year. snowflakes. santa claus bring gifts!");

        Wrapper replace(&doc, &Document::replaceWords,
            { {"from", std::string{"snowflakes"}}, {"to", std::string{"lights"}} });

        Wrapper find(&doc, &Document::findPhrases,
            { {"phrase", std::string{"santa claus"}} });

        Wrapper capitalize_words(&doc, &Document::capitalizeWords, {});
        Wrapper capitalize_sentences(&doc, &Document::capitalizeSentences, {});
        Wrapper number_sentences(&doc, &Document::numberSentences, {});

        Engine engine;
        engine.register_command(&replace, "replace");
        engine.register_command(&find, "find");
        engine.register_command(&capitalize_words, "words");
        engine.register_command(&capitalize_sentences, "sentences");
        engine.register_command(&number_sentences, "number");

        std::cout << engine.execute("replace", { {"to", std::string{"stars"}} }) << "\n";
        std::cout << engine.execute("find", {}) << "\n";
        std::cout << engine.execute("words", {}) << "\n";
        std::cout << engine.execute("sentences", {}) << "\n";
        std::cout << engine.execute("number", {}) << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "unluck" << e.what() << '\n';
        return 1;
    }

    return 0;
}
