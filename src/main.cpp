#include <iostream>
#include "engine.h"
#include "subject.h"
#include "wrapper.h"


void run_text_example() {
    Subject doc("in a hole in the ground there lived a hobbit. not a nasty, dirty, wet hole, filled with the ends of worms and an oozy smell, nor yet a dry, bare, sandy hole with nothing in it to sit down on or to eat. it was a hobbit hole, and that means comfort.");

    Wrapper replace(&doc, &Subject::replace_words, {{"from", ""}, {"to", ""}});
    Wrapper find(&doc, &Subject::find_phrase, {{"phrase", ""}});
    Wrapper cap_words(&doc, &Subject::capitalize_words, {});
    Wrapper cap_sentences(&doc, &Subject::capitalize_sentences, {});
    Wrapper numbering(&doc, &Subject::number_sentences, {});
    Wrapper get_text(&doc, &Subject::get_text, {});

    Engine engine;
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");
    engine.register_command(&cap_words, "capitalize_words");
    engine.register_command(&cap_sentences, "capitalize_sentences");
    engine.register_command(&numbering, "number");
    engine.register_command(&get_text, "text");


    std::cout << "find 'hole': " << engine.execute("find", {{"phrase", "hole"}}) << "\n\n";
    std::cout << engine.execute("replace", {{"from", "hobbit"}, {"to", "dwarf"}}) << "\n\n";
    std::cout << engine.execute("capitalize_sentences") << "\n\n";
    std::cout << engine.execute("capitalize_words") << "\n\n";
    std::cout << engine.execute("number") << '\n';
}


int main() {
    try {
        run_text_example();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}