#include <iostream>
#include <string>
#include "wrapper.h"
#include "engine.h"
#include "text_processor.h"

int main() {
    try {
    TextProcessor processor;
    Engine engine;

    Wrapper<TextProcessor, std::string, const std::string&, const std::string&, const std::string&>
        replaceWrapper(&processor, &TextProcessor::replaceWords,
            { "text", "from", "to" },
            { {"text", std::string("The cat sat on the pillow.")},
             {"from", std::string("cat")},
             {"to", std::string("dog")} });
    engine.registerCommand(&replaceWrapper, "replace");

    std::cout << "replace:\n" << engine.execute("replace") << "\n\n";

    Wrapper<TextProcessor, std::string, const std::string&, const std::string&>
        findWrapper(&processor, &TextProcessor::findPhrase,
            { "text", "phrase" },
            { {"text", std::string("The cat sat on the pillow.")},
             {"phrase", std::string("sat on")} });
    engine.registerCommand(&findWrapper, "find");

    std::cout << "find:\n" << engine.execute("find") << "\n\n";

    Wrapper<TextProcessor, std::string, const std::string&>
        capWordsWrapper(&processor, &TextProcessor::capitalizeWords,
            { "text" },
            { {"text", std::string("the cat sat on the pillow.")} });
    engine.registerCommand(&capWordsWrapper, "capwords");

    std::cout << "capwords:\n" << engine.execute("capwords") << "\n\n";

    Wrapper<TextProcessor, std::string, const std::string&>
        capSentWrapper(&processor, &TextProcessor::capitalizeSentences,
            { "text" },
            { {"text", std::string("the cat sat on the pillow. the dog sat on the pillow.")} });
    engine.registerCommand(&capSentWrapper, "capsent");

    std::cout << "capsent:\n" << engine.execute("capsent") << "\n\n";

    Wrapper<TextProcessor, std::string, const std::string&>
        numberWrapper(&processor, &TextProcessor::numberSentences,
            { "text" },
            { {"text", std::string("First sentence. Second sentence. Third sentence.")} });
    engine.registerCommand(&numberWrapper, "number");

    std::cout << "number:\n" << engine.execute("number");

    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown exception caught\n";
        return 1;
    }
    return 0;
}