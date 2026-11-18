#include<iostream>
#include "Engine.h"
#include "TextProcessor.h"

void test()
{
    TextProcessor txtprc("hello world. this is a test! is it working?");

    Wrapper replace_w(&txtprc, &TextProcessor::replace_words, {{"from", "world"}, {"to", "there"}});
    Wrapper find_w(&txtprc, &TextProcessor::find_phrase, {{"phrase", "test"}});
    Wrapper cap_words_w(&txtprc, &TextProcessor::capitalize_words, {});
    Wrapper cap_sent_w(&txtprc, &TextProcessor::capitalize_sentences, {});
    Wrapper number_w(&txtprc, &TextProcessor::number_sentences, {});

    Engine engine;
    engine.register_command(&replace_w, "replace");
    engine.register_command(&find_w, "find");
    engine.register_command(&cap_words_w,"capitalize_words");
    engine.register_command(&cap_sent_w, "capitalize_sentences");
    engine.register_command(&number_w, "number");

    std::cout << "=== replace (default: world -> there) ===\n" << engine.execute("replace") << "\n\n";

    std::cout << "=== replace (override: hello -> goodbye) ===\n" << engine.execute("replace", {{"from", "hello"}, {"to", "goodbye"}}) << "\n\n";

    std::cout << "=== find (default) ===\n" << engine.execute("find") << "\n\n";

    std::cout << "=== find (override) ===\n" << engine.execute("find", {{"phrase", "nonexistent"}}) << "\n\n";

    std::cout << "=== capitalize_words ===\n" << engine.execute("capitalize_words") << "\n\n";

    std::cout << "=== capitalize_sentences ===\n" << engine.execute("capitalize_sentences") << "\n\n";

    std::cout << "=== number sentences ===\n" << engine.execute("number") << "\n\n";
}

int main()
{
    try
    {
        test();
    }
    catch (const std::exception& e)
    {
        std::cout << "FATAL ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}