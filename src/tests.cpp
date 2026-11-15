// При ошибке код возврата 1, и CI это заметит.
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Engine.h"
#include "TextDocument.h"
#include "Value.h"
#include "Wrapper.h"

namespace {

class Checker {
public:
    void expect(bool condition, const std::string& name) {
        ++checked_;
        if (!condition) {
            ++failed_;
            std::cerr << "FAILED: " << name << '\n';
        }
    }

    void expect_throws(const std::function<void()>& action, const std::string& name) {
        bool thrown = false;
        try {
            action();
        } catch (const std::invalid_argument&) {
            thrown = true;
        }
        expect(thrown, name);
    }

    [[nodiscard]] int exit_code() const {
        std::cout << checked_ - failed_ << " of " << checked_ << " checks passed\n";
        return failed_ == 0 ? 0 : 1;
    }

private:
    int checked_ = 0;
    int failed_ = 0;
};

class Counter {
public:
    int add(int amount) {
        total_ += amount;
        return total_;
    }

    [[nodiscard]] std::string label(const std::string& prefix, bool loud) const {
        return prefix + ": " + std::to_string(total_) + (loud ? "!" : "");
    }

private:
    int total_ = 0;
};

void test_value(Checker& check) {
    check.expect(Value(0).text() == "0", "number is stored as text");
    check.expect(Value("a").text() == "a", "string is stored as is");
    check.expect(Value(true).text() == "true", "bool is stored as true/false");
    check.expect(Value("-3").as<int>() == -3, "text converts to int");
    check.expect(Value(" 4 ").as<long>() == 4, "spaces around a number are allowed");
    check.expect(!Value("false").as<bool>(), "text converts to bool");
    check.expect(Value("x y").as<std::string>() == "x y", "text converts to string");
    check.expect_throws([] { static_cast<void>(Value("abc").as<int>()); }, "not a number");
    check.expect_throws([] { static_cast<void>(Value("4x").as<int>()); }, "number with a tail");
    check.expect_throws([] { static_cast<void>(Value("yes").as<bool>()); }, "not a boolean");
    check.expect_throws([] { static_cast<void>(Value(static_cast<const char*>(nullptr))); }, "null string");
}

void test_wrapper(Checker& check) {
    Counter counter;
    Wrapper add(&counter, &Counter::add, {{"amount", 1}});
    check.expect(add.call({}) == "1", "default value is used");
    check.expect(add.call({{"amount", 3}}) == "4", "new value replaces default, object keeps state");
    check.expect(add.call({}) == "5", "default value is kept after a call with a new value");

    Wrapper label(&counter, &Counter::label, {{"prefix", "total"}, {"loud", false}});
    check.expect(label.call({}) == "total: 5", "const method with defaults");
    check.expect(label.call({{"loud", true}}) == "total: 5!", "part of arguments replaced");
    check.expect(label.call({{"loud", "true"}, {"prefix", "sum"}}) == "sum: 5!", "order of new values does not matter");

    check.expect_throws([&add] { static_cast<void>(add.call({{"value", 1}})); }, "unknown argument name");
    check.expect_throws([&add] { static_cast<void>(add.call({{"amount", "many"}})); }, "value of wrong type");
    check.expect_throws([&counter] { Wrapper wrapper(&counter, &Counter::add, {}); }, "too few default values");
    check.expect_throws(
        [&counter] { Wrapper wrapper(&counter, &Counter::label, {{"prefix", "a"}, {"prefix", "b"}}); },
        "argument listed twice");
    check.expect_throws([] { Wrapper wrapper(static_cast<Counter*>(nullptr), &Counter::add, {{"amount", 1}}); },
                        "null object");
}

void test_engine(Checker& check) {
    Counter counter;
    Wrapper add(&counter, &Counter::add, {{"amount", 2}});
    Engine engine;
    engine.register_command(&add, "add");

    check.expect(engine.execute("add") == "2", "execute with default values");
    check.expect(engine.execute("add", {{"amount", 3}}) == "5", "execute with new values");
    check.expect_throws([&engine] { static_cast<void>(engine.execute("sub")); }, "unknown command");
    check.expect_throws([&engine, &add] { engine.register_command(&add, "add"); }, "name already used");
    check.expect_throws([&engine] { engine.register_command(nullptr, "none"); }, "null command");
    check.expect_throws([&engine, &add] { engine.register_command(&add, ""); }, "empty name");
}

void test_text_document(Checker& check) {
    TextDocument words("a cat and a catalog, cat!");
    check.expect(words.replace_word("cat", "dog") == "a dog and a catalog, dog!", "whole words are replaced");
    check.expect_throws([&words] { words.replace_word("two words", "x"); }, "replace accepts one word");
    check.expect_throws([&words] { words.replace_word("", "x"); }, "replace rejects empty word");

    TextDocument russian("привет, мир! мирный день");
    check.expect(russian.replace_word("мир", "друг") == "привет, друг! мирный день", "replace in UTF-8 text");

    const TextDocument phrases("the lazy\n  dog sleeps");
    check.expect(phrases.find_phrase("lazy dog") == "found", "phrase found across spaces and lines");
    check.expect(phrases.find_phrase("lazy do") == "not found", "phrase must end on a word boundary");
    check.expect(phrases.find_phrase("azy dog") == "not found", "phrase must start on a word boundary");
    check.expect_throws([&phrases] { static_cast<void>(phrases.find_phrase("  ")); }, "empty phrase");

    TextDocument title("hello world, don't stop_now");
    check.expect(title.capitalize_words() == "Hello World, Don't Stop_now", "first letter of every word");

    TextDocument sentences("first one. second? third! pi is 3.14 here. end");
    check.expect(sentences.capitalize_sentences() == "First one. Second? Third! Pi is 3.14 here. End",
                 "first letter of every sentence");

    TextDocument numbered("one.  two!\nthree?! four");
    check.expect(numbered.number_sentences() == "1. one.\n2. two!\n3. three?!\n4. four",
                 "sentences are numbered, each on a new line");
}

}  // namespace

int main() {
    Checker check;
    try {
        test_value(check);
        test_wrapper(check);
        test_engine(check);
        test_text_document(check);
    } catch (const std::exception& error) {
        std::cerr << "unexpected exception: " << error.what() << '\n';
        return 1;
    }
    return check.exit_code();
}
