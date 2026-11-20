#include "TextProcessor.h"
#include <cctype>
#include <sstream>
#include <string>


TextProcessor::TextProcessor(std::string text) : my_text(std::move(text)) {}

// Replaces every instance of old_word to new_word
std::string TextProcessor::replace_words(const std::string& old_word, const std::string& new_word) {
    if (old_word.empty()) {
        return my_text;
    }

    std::size_t position = 0;
    // Loop throgh whole string
    while ((position = my_text.find(old_word, position)) != std::string::npos) {
        std::size_t end = position + old_word.length();

        // Check, that we are replacing the whole word (to left and to right of found word - not character or number)
        bool left_boundary_correct = (position == 0) || !static_cast<bool>(std::isalnum(static_cast<unsigned char>(my_text[position - 1])));

        bool right_boundary_correct = (end == my_text.length()) || !static_cast<bool>(std::isalnum(static_cast<unsigned char>(my_text[end])));
        
        if (left_boundary_correct && right_boundary_correct) {
            my_text.replace(position, old_word.length(), new_word);
            position += new_word.length(); // Continue searching for replacement from here
        } else {
            position += old_word.length();
        }
    }

    return my_text;
}

// Search for string (returns "Found" or "Not found")
std::string_view TextProcessor::find_phrase(const std::string& phrase) const {
    if (my_text.find(phrase) != std::string::npos) {
        return "Found";
    }
    return "Not found";
}

// Capitalizes first letter of each word (returns resulting string)
std::string TextProcessor::capitalize_each_word() {

    bool new_word = true;
    for (std::size_t i = 0; i < my_text.length(); ++i) {
        unsigned char character = my_text[i];

        if (static_cast<bool>(std::isalpha(character))) {
            if (new_word) {
                my_text[i] = static_cast<char>(std::toupper(character));
            }
            new_word = false; // Переключаем флаг, чтобы переводить во верхний регистр только первую букву
        } else {
            new_word = true;
        }
    }
    return my_text;
}

// Capitalizes first letter of each sentence (returns resulting string)
std::string TextProcessor::capitalize_sentence_starts() {

    bool new_sentence = true;
    for (std::size_t i = 0; i < my_text.length(); ++i) {
        unsigned char character = my_text[i];

        if (static_cast<bool>(std::isalpha(character)) && new_sentence) {
            my_text[i] = static_cast<char>(std::toupper(character));
            new_sentence = false;
        }
        if (character == '.' || character == '!' || character == '?') {
            new_sentence = true;
        }
    }
    return my_text;
}

// Add numbering to sentences (returns resulting string)
std::string TextProcessor::number_sentences() {
    std::stringstream result;

    int sentence_number = 1;
    bool sentence_start = true;
    for (char character : my_text) {
        if (sentence_start) {
            // Skip whitespaces before sentences
            if (static_cast<bool>(std::isspace(character))) { continue;}

            result << sentence_number++ << ". ";
            sentence_start = false;
        }

        result << character;
        if (character == '.' || character == '!' || character == '?') {
            result << '\n';
            sentence_start = true;
        }
    }
    my_text = result.str();
    return my_text;
}

// Returns current string
const std::string& TextProcessor::text() const{
    return my_text;
}
