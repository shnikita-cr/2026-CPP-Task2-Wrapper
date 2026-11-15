#include "engine.h"
#include "text_processor.h"
#include <iostream>

namespace {
void demonstrate() {
    TextProcessor processor("the report is ready. send it on monday.");
    const Wrapper replace(&processor, &TextProcessor::replace_words,
                          {{"word", "monday"}, {"replacement", "tuesday"}});
    const Wrapper search(&processor, &TextProcessor::find_phrase,
                         {{"phrase", "report is ready"}});
    const Wrapper words(&processor, &TextProcessor::capitalize_words, {});
    const Wrapper sentences(&processor, &TextProcessor::capitalize_sentences, {});
    const Wrapper numbering(&processor, &TextProcessor::number_sentences, {});
    Engine engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&search, "search");
    engine.register_command(&words, "words");
    engine.register_command(&sentences, "sentences");
    engine.register_command(&numbering, "number");
    std::cout << "Original: " << processor.text() << '\n'
              << "Replace: " << engine.execute("replace", {{"replacement", "friday"}}) << '\n'
              << "Search: " << engine.execute("search") << '\n'
              << "Words: " << engine.execute("words") << '\n'
              << "Sentences: " << engine.execute("sentences") << '\n'
              << "Numbered:\n" << engine.execute("number") << '\n';
}

}

int main() {
    try {
        demonstrate();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
