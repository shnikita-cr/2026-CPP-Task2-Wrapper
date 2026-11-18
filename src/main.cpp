#include "TextProcessor.h"
#include <exception>
#include <iostream>
#include <string>

int main() {
    try {
        TextProcessor text("   hello world.   \n How is world? \n \tbYe world.");
        
        std::cout << "Initial text:\n" << text.text() << "\n\n";
        std::cout << "After replacement:\n" << text.replace_words("world", "program") << "\n\n";
        std::cout << "Phrase search (searching for \"world\"): " << text.find_phrase("world") << "\n\n";
        std::cout << "Capitalized sentence starts:\n" << text.capitalize_sentence_starts() << "\n\n";
        std::cout << "Capialized words:\n" << text.capitalize_each_word() << "\n\n";
        std::cout << "Numbered sentences:\n" << text.number_sentences() << '\n';

    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
