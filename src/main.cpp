#include "Document.h"
#include "Engine.h"
#include "Wrapper.h"
#include <exception>
#include <iostream>

int main() {
    try {
        Document document("hello world. abc def! hello test. qwe rty?");

        std::cout << document.get_text() << '\n';

        Wrapper replace_wrap(&document, &Document::replace_words,
                             {{"old_word", "test"}, {"new_word", "123"}});
        Wrapper find_wrap(&document, &Document::find_phrase,
                          {{"phrase", "hello"}});
        Wrapper cap_sent_wrap(&document, &Document::capitalize_sentences,
                              {{"delimiters", ".!?"}});
        Wrapper cap_words_wrap(&document, &Document::capitalize_words,
                               {{"target_word", ""}});
        Wrapper num_sent_wrap(&document, &Document::number_sentences,
                              {{"separator", ". "}});

        Engine engine;
        engine.register_command(&find_wrap, "find");
        engine.register_command(&replace_wrap, "replace");
        engine.register_command(&cap_sent_wrap, "cap_sent");
        engine.register_command(&cap_words_wrap, "cap_words");
        engine.register_command(&num_sent_wrap, "num_sent");

        std::cout << engine.execute("find") << '\n';
        std::cout << engine.execute("find", {{"phrase", "abc def"}}) << '\n';

        std::cout << engine.execute("replace") << '\n';
        std::cout << engine.execute("replace", {{"old_word", "qwe"}, {"new_word", "zzz"}}) << '\n';

        std::cout << engine.execute("cap_sent") << '\n';
        std::cout << engine.execute("cap_words") << '\n';
        std::cout << engine.execute("num_sent", {{"separator", ") "}}) << '\n';
    } catch (const std::exception& err) {
        std::cerr << err.what() << '\n';
        return 1;
    }

    return 0;
}
