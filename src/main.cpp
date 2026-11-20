#include <exception>
#include <iostream>

#include "engine.h"
#include "text_processor.h"

int main() {
    try {
        TextProcessor processor;
        Wrapper replaceWords(&processor, &TextProcessor::replaceWords,
                             {{"text", "the cat sat beside the catalog."},
                              {"word", "cat"},
                              {"replacement", "dog"}});
        Wrapper findPhrase(&processor, &TextProcessor::findPhrase,
                           {{"arg1", "A wrapper calls a command through the engine."},
                            {"arg2", "through the engine"}});
        Wrapper capitalizeWords(&processor, &TextProcessor::capitalizeEachWord,
                                {{"text", "текстовый обработчик работает"}});
        Wrapper capitalizeSentences(&processor,
                                    &TextProcessor::capitalizeSentenceStarts,
                                    {{"text", "первая фраза. вторая фраза! третья?"}});
        Wrapper numberSentences(&processor, &TextProcessor::numberEachSentence,
                                {{"text", "First sentence. Second sentence! Third?"}});

        Engine engine;
        engine.register_command(&replaceWords, "replace_words");
        engine.register_command(&findPhrase, "find_phrase");
        engine.register_command(&capitalizeWords, "capitalize_words");
        engine.register_command(&capitalizeSentences, "capitalize_sentences");
        engine.register_command(&numberSentences, "number_sentences");

        std::cout << "Replace: "
                  << engine.execute("replace_words",
                                    {{"word", "the"}, {"replacement", "a"}})
                  << '\n';
        std::cout << "Search: " << engine.execute("find_phrase") << '\n';
        std::cout << "Capitalize words: "
                  << engine.execute("capitalize_words") << '\n';
        std::cout << "Capitalize sentences: "
                  << engine.execute("capitalize_sentences") << '\n';
        std::cout << "Number sentences:\n"
                  << engine.execute("number_sentences") << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
