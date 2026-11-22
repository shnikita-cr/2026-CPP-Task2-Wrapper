#include <iostream>
#include <memory>
#include <fstream>
#include <exception>
#include "Wrapper.h"
#include "Subject.h"
#include "Engine.h"

int main() {

    // ========================================
    // 1. WORD REPLACEMENT (replaceWord)
    // ========================================
    std::cout << "1. WORD REPLACEMENT\n";
    std::cout << "-------------------\n";

    {
        std::ofstream file("replace_test.txt");
        file << "The quick brown fox jumps over the lazy dog.\n";
        file << "The fox is very quick and the dog is very lazy.\n";
    }

    std::cout << "Initial file replace_test.txt:\n";
    std::cout << "The quick brown fox jumps over the lazy dog.\n";
    std::cout << "The fox is very quick and the dog is very lazy.\n\n";

    try {
        Wrapper wrapper1(std::make_unique<Subject>("replace_test.txt"), &Subject::replaceWord, { {"target", "fox"}, {"replacement", "cat"} });
        Engine engine1;
        engine1.register_command(&wrapper1, "replace");
        engine1.execute("replace", { {"target", "fox"}, {"replacement", "cat"} });
    }
    catch (const std::exception& e) {
        std::cerr << "Error in replace block: " << e.what() << "\n";
    }

    std::cout << "Command: replace(target='fox', replacement='cat')\n";
    std::cout << "Result in file replace_test.txt:\n";
    std::cout << "The quick brown cat jumps over the lazy dog.\n";
    std::cout << "The cat is very quick and the dog is very lazy.\n\n";

    // ========================================
    // 2. PHRASE SEARCH (findPhrase)
    // ========================================
    std::cout << "2. PHRASE SEARCH\n";
    std::cout << "----------------\n";

    {
        std::ofstream file("find_test.txt");
        file << "Hello world! This is a test document.\n";
        file << "We are testing the find phrase functionality.\n";
        file << "Can you find the phrase 'test document'?\n";
    }

    std::cout << "Initial file find_test.txt:\n";
    std::cout << "Hello world! This is a test document.\n";
    std::cout << "We are testing the find phrase functionality.\n";
    std::cout << "Can you find the phrase 'test document'?\n\n";

    std::string result1;
    std::string result2;
    try {
        Wrapper wrapper2(std::make_unique<Subject>("find_test.txt"), &Subject::findPhrase, { {"phrase", "test document"} });
        Engine engine2;
        engine2.register_command(&wrapper2, "find");
        result1 = engine2.execute("find", { {"phrase", "test document"} });
        result2 = engine2.execute("find", { {"phrase", "not found phrase"} });
    }
    catch (const std::exception& e) {
        std::cerr << "Error in find block: " << e.what() << "\n";
    }

    std::cout << "Command: find(phrase='test document')\n";
    std::cout << "Result: " << (result1 == "1" ? "Found (true)" : "Not found (false)") << "\n\n";

    std::cout << "Command: find(phrase='not found phrase')\n";
    std::cout << "Result: " << (result2 == "1" ? "Found (true)" : "Not found (false)") << "\n\n";

    // ========================================
    // 3. CAPITALIZE EVERY WORD (capitalizeWords)
    // ========================================
    std::cout << "3. CAPITALIZE EVERY WORD\n";
    std::cout << "------------------------\n";

    {
        std::ofstream file("capitalize_words_test.txt");
        file << "this is a test sentence with lowercase words.\n";
        file << "another line with multiple words to capitalize.\n";
    }

    std::cout << "Initial file capitalize_words_test.txt:\n";
    std::cout << "this is a test sentence with lowercase words.\n";
    std::cout << "another line with multiple words to capitalize.\n\n";

    try {
        Wrapper wrapper3(std::make_unique<Subject>("capitalize_words_test.txt"), &Subject::capitalizeWords, {});
        Engine engine3;
        engine3.register_command(&wrapper3, "capitalize_words");
        engine3.execute("capitalize_words", {});
    }
    catch (const std::exception& e) {
        std::cerr << "Error in capitalize_words block: " << e.what() << "\n";
    }

    std::cout << "Command: capitalize_words()\n";
    std::cout << "Result in file capitalize_words_test.txt:\n";
    std::cout << "This Is A Test Sentence With Lowercase Words.\n";
    std::cout << "Another Line With Multiple Words To Capitalize.\n\n";

    // ========================================
    // 4. CAPITALIZE FIRST LETTER OF SENTENCE (capitalizeSentences)
    // ========================================
    std::cout << "4. CAPITALIZE FIRST LETTER OF SENTENCE\n";
    std::cout << "--------------------------------------\n";

    {
        std::ofstream file("capitalize_sentences_test.txt");
        file << "this is the first sentence. this is the second one. and this is the third!\n";
        file << "here we go with another line. it has multiple sentences too. aren't they great?\n";
    }

    std::cout << "Initial file capitalize_sentences_test.txt:\n";
    std::cout << "this is the first sentence. this is the second one. and this is the third!\n";
    std::cout << "here we go with another line. it has multiple sentences too. aren't they great?\n\n";

    try {
        Wrapper wrapper4(std::make_unique<Subject>("capitalize_sentences_test.txt"), &Subject::capitalizeSentences, {});
        Engine engine4;
        engine4.register_command(&wrapper4, "capitalize_sentences");
        engine4.execute("capitalize_sentences", {});
    }
    catch (const std::exception& e) {
        std::cerr << "Error in capitalize_sentences block: " << e.what() << "\n";
    }

    std::cout << "Command: capitalize_sentences()\n";
    std::cout << "Result in file capitalize_sentences_test.txt:\n";
    std::cout << "This is the first sentence. This is the second one. And this is the third!\n";
    std::cout << "Here we go with another line. It has multiple sentences too. Aren't they great?\n\n";

    // ========================================
    // 5. SENTENCE NUMBERING (getNumberedSentences)
    // ========================================
    std::cout << "5. SENTENCE NUMBERING\n";
    std::cout << "---------------------\n";

    {
        std::ofstream file("numbered_test.txt");
        file << "First sentence here. Second one follows. Third is the last!\n";
        file << "New line starts. Another sentence. Final words.\n";
    }

    std::cout << "Initial file numbered_test.txt:\n";
    std::cout << "First sentence here. Second one follows. Third is the last!\n";
    std::cout << "New line starts. Another sentence. Final words.\n\n";

    std::string result;
    try {
        Wrapper wrapper5(std::make_unique<Subject>("numbered_test.txt"), &Subject::getNumberedSentences, {});
        Engine engine5;
        engine5.register_command(&wrapper5, "number");
        result = engine5.execute("number", {});
    }
    catch (const std::exception& e) {
        std::cerr << "Error in number block: " << e.what() << "\n";
    }

    std::cout << "Command: getNumberedSentences()\n";
    std::cout << "Result (returned as string):\n";
    std::cout << result << "\n";

    return 0;
}