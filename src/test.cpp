#include "Document.h"
#include "Engine.h"

#include <iostream>
#include <stdexcept>
#include <string>

#define CHECK(condition) do { \
    if (!(condition)) { \
        std::cerr << "Test failed at line " << __LINE__ << ": " #condition << '\n'; \
        return false; \
    } \
} while (false)

namespace {
struct Subject {
    std::string separator = " ";
    [[nodiscard]] std::string func(const Arguments& args) const {
        return args.at("arg1") + separator + args.at("arg2");
    }
};

struct ConstSubject {
    std::string prefix;
    [[nodiscard]] std::string func(const Arguments& args) const {
        return prefix + args.at("x");
    }
};

bool test_wrapper_and_engine() {
    Subject subj;
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", "0"}, {"arg2", "a"}});
    Engine engine;
    engine.register_command(&wrapper, "command_name");
    CHECK(engine.execute("command_name", {{"arg1", "4"}}) == "4 a");
    CHECK(engine.execute("command_name", {{"arg2", "b"}}) == "0 b");
    CHECK(engine.execute("command_name") == "0 a");

    const ConstSubject const_subj;
    Wrapper const_wrapper(&const_subj, &ConstSubject::func, {{"x", "ok"}});
    engine.register_command(&const_wrapper, "const_command");
    CHECK(engine.execute("const_command") == "ok");

    bool caught = false;
    try {
        static_cast<void>(engine.execute("unknown"));
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught);

    caught = false;
    try {
        engine.register_command(&wrapper, "command_name");
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    CHECK(caught);
    return true;
}

bool test_replacement_boundaries() {
    Document small("а");
    CHECK(small.replace_words({{"from", "кот"}, {"to", "пёс"}}) == "а");
    Document two_letters("аб");
    CHECK(two_letters.replace_words({{"from", "кот"}, {"to", "пёс"}}) == "аб");
    Document at_end("мой кот");
    CHECK(at_end.replace_words({{"from", "кот"}, {"to", "пёс"}}) == "мой пёс");
    Document partial("котик");
    CHECK(partial.replace_words({{"from", "кот"}, {"to", "пёс"}}) == "котик");
    Document empty("");
    CHECK(empty.replace_words({{"from", "кот"}, {"to", "пёс"}}).empty());
    Document long_word("а");
    CHECK(long_word.replace_words({{"from", "оченьдлинноеслово"},
                                   {"to", "x"}}) == "а");

    return true;
}

bool test_replacement() {
    Document document("кот котик кот. Кот и кот!");
    Wrapper wrapper(&document, &Document::replace_words,
                    {{"from", "кот"}, {"to", "пёс"}});
    CHECK(wrapper.invoke() == "пёс котик пёс. Кот и пёс!");
    CHECK(wrapper.invoke({{"from", "пёс"}, {"to", "ёж"}}) ==
          "ёж котик ёж. Кот и ёж!");

    // Два Enter в меню означают использование обоих аргументов по умолчанию.
    Document unchanged("добрый день!");
    Wrapper default_wrapper(&unchanged, &Document::replace_words,
                            {{"from", "кот"}, {"to", "пёс"}});
    CHECK(default_wrapper.invoke() == "добрый день!");
    return true;
}

bool test_search() {
    Document document("мама и мама");
    Wrapper wrapper(&document, &Document::find_phrase, {{"phrase", "мама"}});
    CHECK(wrapper.invoke() == "Найдено");
    CHECK(wrapper.invoke({{"phrase", "папа"}}) == "Не найдено");
    CHECK(document.text() == "мама и мама");
    return true;
}

bool test_capitalization() {
    Document words("привет, мир! ёж и кот");
    CHECK(words.capitalize_words({}) == "Привет, Мир! Ёж И Кот");
    Document sentences("привет! как дела? всё хорошо.\nновая строка");
    CHECK(sentences.capitalize_sentences({}) ==
          "Привет! Как дела? Всё хорошо.\nНовая строка");
    return true;
}

bool test_numbering() {
    Document document("Первое предложение. Второе? Третье! Ещё одно...");
    CHECK(document.number_sentences({}) ==
          "1. Первое предложение.\n2. Второе?\n3. Третье!\n4. Ещё одно...");
    Document empty("  \n  ");
    CHECK(empty.number_sentences({}).empty());
    Document one("без точки");
    CHECK(one.number_sentences({}) == "1. без точки");
    Document emoji("привет 🌍! ёж 🐱.");
    CHECK(emoji.capitalize_sentences({}) == "Привет 🌍! Ёж 🐱.");
    Document unicode("ёжик 🌍 🐱");
    CHECK(unicode.replace_words({{"from", "ёжик"}, {"to", "ёж"}}) ==
          "ёж 🌍 🐱");
    return true;
}

bool test_invalid_arguments() {
    Document document("текст");
    bool caught = false;
    try {
        document.replace_words({{"from", ""}, {"to", "x"}});
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    CHECK(caught);
    caught = false;
    try {
        static_cast<void>(document.find_phrase({{"phrase", ""}}));
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    CHECK(caught);
    return true;
}
}  // namespace

int main() {
    try {
        if (!test_wrapper_and_engine() || !test_replacement() ||
            !test_replacement_boundaries() || !test_search() ||
            !test_capitalization() || !test_numbering() || !test_invalid_arguments()) {
            return 1;
        }
        std::cout << "All tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Unexpected exception: " << error.what() << '\n';
        return 1;
    }
}
