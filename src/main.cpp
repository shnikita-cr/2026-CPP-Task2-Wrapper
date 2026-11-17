#include "engine.h"
#include "placeholder.h"
#include "text_processor.h"
#include "wrapper.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

static void check_condition(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "Assertion failed: " << message << '\n';
        std::exit(1);
    }
}

#define REQUIRE(condition) check_condition(static_cast<bool>(condition), #condition)

// NOLINTBEGIN(readability-magic-numbers)

// Test subject matching the exact example in the README
class Subject {
private:
    int bias_ = 0;

public:
    // NOLINTBEGIN(bugprone-easily-swappable-parameters)
    [[nodiscard]] int func(int arg1, int arg2) const {
        return arg1 + arg2 + bias_;
    }
    // NOLINTEND(bugprone-easily-swappable-parameters)
};

static void run_readme_example() {
    std::cout << "=== Running README Example ===\n";
    Subject subj;
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", 0}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");
    std::cout << engine.execute("command_name", {{"arg1", 4}, {"arg2", 5}}) << '\n';

    REQUIRE(engine.execute("command_name", {{"arg1", 4}, {"arg2", 5}}) == "9");
    REQUIRE(engine.execute("command_name", {{"arg1", 10}}) == "10");
    REQUIRE(engine.execute("command_name", {{"arg2", 7}}) == "7");
    REQUIRE(engine.execute("command_name") == "0");
}

static void run_text_processor_demonstration() {
    std::cout << "\n=== Running TextProcessor Demonstration ===\n";

    TextProcessor processor;
    Engine engine;

    // 1. Замена слов в тексте
    Wrapper w_replace(
        &processor,
        &TextProcessor::replace_words,
        {{"text", ""}, {"old_word", ""}, {"new_word", ""}});
    engine.register_command(&w_replace, "replace_words");

    // 2. Поиск словосочетаний
    Wrapper w_find(
        &processor,
        &TextProcessor::find_phrase,
        {{"text", ""}, {"phrase", ""}});
    engine.register_command(&w_find, "find_phrase");

    // 3. Изменение первых букв слов на заглавные
    Wrapper w_cap_words(
        &processor,
        &TextProcessor::capitalize_words,
        {{"text", ""}});
    engine.register_command(&w_cap_words, "capitalize_words");

    // 4. Изменение первой буквы предложения на заглавную
    Wrapper w_cap_sentences(
        &processor,
        &TextProcessor::capitalize_sentences,
        {{"text", ""}});
    engine.register_command(&w_cap_sentences, "capitalize_sentences");

    // 5. Добавление последовательной нумерации к каждому предложению
    Wrapper w_num_sentences(
        &processor,
        &TextProcessor::number_sentences,
        {{"text", ""}});
    engine.register_command(&w_num_sentences, "number_sentences");

    // Demonstration 1: Replace words
    std::string sample_text =
        "The quick brown fox jumps over the lazy dog. A wild dog barks.";
    std::string replaced = engine.execute("replace_words", {
        {"text", sample_text},
        {"old_word", "dog"},
        {"new_word", "cat"}
    });
    std::cout << "1. replace_words result:\n   " << replaced << '\n';
    REQUIRE(replaced == "The quick brown fox jumps over the lazy cat. A wild cat barks.");

    // Demonstration 2: Find phrase
    std::string found_status = engine.execute("find_phrase", {
        {"text", sample_text},
        {"phrase", "lazy dog"}
    });
    std::string not_found_status = engine.execute("find_phrase", {
        {"text", sample_text},
        {"phrase", "blue bird"}
    });
    std::cout << "2. find_phrase results:\n   'lazy dog': " << found_status
              << "\n   'blue bird': " << not_found_status << '\n';
    REQUIRE(found_status == "найдено");
    REQUIRE(not_found_status == "не найдено");

    // Demonstration 3: Capitalize words
    std::string cap_words_res = engine.execute("capitalize_words", {
        {"text", "hello world, modern c++ architecture is great"}
    });
    std::cout << "3. capitalize_words result:\n   " << cap_words_res << '\n';
    REQUIRE(cap_words_res == "Hello World, Modern C++ Architecture Is Great");

    // Demonstration 4: Capitalize sentences
    std::string cap_sentences_res = engine.execute("capitalize_sentences", {
        {"text", "first sentence. second sentence! is this third? yes."}
    });
    std::cout << "4. capitalize_sentences result:\n   " << cap_sentences_res << '\n';
    REQUIRE(cap_sentences_res == "First sentence. Second sentence! Is this third? Yes.");

    // Demonstration 5: Number sentences
    std::string doc =
        "Text modification document loaded. Wrapper pattern encapsulates methods! "
        "Engine executes by string alias. Sequential numbering applied.";
    std::string numbered = engine.execute("number_sentences", {{"text", doc}});
    std::cout << "5. number_sentences result:\n" << numbered << '\n';
    REQUIRE(numbered ==
        "1. Text modification document loaded.\n"
        "2. Wrapper pattern encapsulates methods!\n"
        "3. Engine executes by string alias.\n"
        "4. Sequential numbering applied.");
}

static void run_architectural_unit_tests() {
    std::cout << "\n=== Running Architectural Unit Tests ===\n";

    TextProcessor processor;
    Wrapper w_replace_partial(
        &processor,
        &TextProcessor::replace_words,
        {{"text", "default apples and apples"}, {"old_word", "apples"}, {"new_word", "oranges"}});

    Engine engine;
    engine.register_command(&w_replace_partial, "test_partial");

    // 1. All defaults
    REQUIRE(engine.execute("test_partial") == "default oranges and oranges");

    // 2. Partial override: only new_word
    REQUIRE(engine.execute("test_partial", {{"new_word", "bananas"}}) ==
           "default bananas and bananas");

    // 3. Partial override: only text
    REQUIRE(engine.execute("test_partial", {{"text", "fresh apples"}}) ==
           "fresh oranges");

    // 4. Overriding all
    REQUIRE(engine.execute("test_partial", {
        {"text", "one two three"},
        {"old_word", "two"},
        {"new_word", "TEN"}
    }) == "one TEN three");

    // Test unknown command exception
    bool caught_unknown = false;
    try {
        engine.execute("non_existent_command");
    } catch (const std::exception&) {
        caught_unknown = true;
    }
    REQUIRE(caught_unknown);

    // Test typed execute template
    Subject subj;
    Wrapper w_math(&subj, &Subject::func, {{"a", 10}, {"b", 20}});
    engine.register_command(&w_math, "add");
    int sum = engine.execute<int>("add", {{"b", 5}});
    REQUIRE(sum == 15);

    // Test copy and move of Wrapper
    Wrapper w_copy = w_math;
    REQUIRE(w_copy.execute({{"a", 1}, {"b", 2}}) == "3");
    Wrapper w_move = std::move(w_copy);
    REQUIRE(w_move.execute({{"a", 2}, {"b", 3}}) == "5");

    // Test Engine utility methods
    REQUIRE(engine.has_command("add"));
    REQUIRE(!engine.has_command("random_name"));
    REQUIRE(engine.size() >= 2);
    REQUIRE(engine.unregister_command("add"));
    REQUIRE(!engine.has_command("add"));

    std::cout << "All architectural tests passed successfully!\n";
}

static void run_utf8_and_edge_case_tests() {
    std::cout << "\n=== Running UTF-8 & Edge-Case Tests ===\n";

    TextProcessor processor;

    // Edge cases for replace_words
    REQUIRE(processor.replace_words("", "word", "rep").empty());
    REQUIRE(processor.replace_words("hello", "", "rep") == "hello");
    REQUIRE(processor.replace_words("cat caterpillar bobcat cat", "cat", "dog") ==
            "dog caterpillar bobcat dog");
    REQUIRE(processor.replace_words("рыжий кот и белый кот сидели на коврике", "кот", "пёс") ==
            "рыжий пёс и белый пёс сидели на коврике");
    REQUIRE(processor.replace_words("котик и кот", "кот", "пёс") == "котик и пёс");

    // Edge cases for find_phrase
    REQUIRE(processor.find_phrase("", "phrase") == "не найдено");
    REQUIRE(processor.find_phrase("some text", "") == "не найдено");
    REQUIRE(processor.find_phrase("В лесу родилась ёлочка", "родилась ёлочка") == "найдено");
    REQUIRE(processor.find_phrase("В лесу родилась ёлочка", "родилась берёзка") == "не найдено");

    // Edge cases for capitalize_words
    REQUIRE(processor.capitalize_words("").empty());
    REQUIRE(processor.capitalize_words("   ") == "   ");
    REQUIRE(processor.capitalize_words("a") == "A");
    REQUIRE(processor.capitalize_words("hello   world\tfoo\nbar") == "Hello   World\tFoo\nBar");
    REQUIRE(processor.capitalize_words("ёжик в тумане") == "Ёжик В Тумане");
    REQUIRE(processor.capitalize_words("one, two; three: four!") == "One, Two; Three: Four!");

    // Edge cases for capitalize_sentences
    REQUIRE(processor.capitalize_sentences("").empty());
    REQUIRE(processor.capitalize_sentences("   hello.") == "   Hello.");
    REQUIRE(processor.capitalize_sentences("hello... world?! yes! fine.") == "Hello... World?! Yes! Fine.");
    REQUIRE(processor.capitalize_sentences("привет... как дела?! все отлично.") ==
            "Привет... Как дела?! Все отлично.");

    // Edge cases for number_sentences
    REQUIRE(processor.number_sentences("").empty());
    REQUIRE(processor.number_sentences("   \n\t   ").empty());
    REQUIRE(processor.number_sentences("Only one sentence") == "1. Only one sentence");
    REQUIRE(processor.number_sentences("Wait... What?! Done.") ==
            "1. Wait...\n2. What?!\n3. Done.");

    std::cout << "All UTF-8 and edge-case tests passed successfully!\n";
}

// NOLINTEND(readability-magic-numbers)

int main() noexcept {
    try {
        InitialArchitecturePlaceholder::projectInitialized();

        run_readme_example();
        run_text_processor_demonstration();
        run_architectural_unit_tests();
        run_utf8_and_edge_case_tests();

        std::cout << "\nAll demonstrations and verifications finished successfully!\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Fatal error: " << exception.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Fatal unknown error\n";
        return 1;
    }
}
