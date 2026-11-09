//
//  main.cpp
//  Wrapper
//
//  Created by Богдан on 08.11.2026.
//
#include "engine.h"
#include "text_document.h"
#include "wrapper.h"

#include <iostream>
#include <memory>
#include <string>

class Subject {
public:
    std::string func(int arg1, const std::string& arg2) {
        return "Subject::func called with arg1=" + std::to_string(arg1) +
               ", arg2=" + arg2;
    }

    std::string func_map(const AnyMap& args) {
        const int a1 = get_arg_or<int>(args, "arg1", 0);
        const std::string a2 =
            get_arg_or<std::string>(args, "arg2", std::string("a"));
        return func(a1, a2);
    }
};

static void run_classic_demo() {
    std::cout << "Part 1: Classic Wrapper / Engine demo\n";

    Subject subj;

    Wrapper wrapper1(&subj, &Subject::func_map,
                     AnyMap{{"arg1", 0}, {"arg2", std::string("a")}});

    Engine engine;
    engine.register_command(&wrapper1, "command_name");

    std::cout << engine.execute("command_name",
                                {{"arg1", 4}, {"arg2", std::string("b")}})
              << '\n';

    auto wrapper2 = make_wrapper(
        &subj, &Subject::func,
        AnyMap{{"arg1", 0}, {"arg2", std::string("a")}},
        [](Subject* s, auto method, const AnyMap& args) -> std::any {
            const int a1 = get_arg_or<int>(args, "arg1", 0);
            const std::string a2 =
                get_arg_or<std::string>(args, "arg2", std::string("a"));
            return (s->*method)(a1, a2);
        });

    engine.register_command(std::move(wrapper2), "typed_command");
    std::cout << engine.execute("typed_command", {{"arg1", 42}}) << '\n';
}

static void run_document_demo() {
    std::cout << "\nPart 2: Text document operations via Engine\n";

    TextDocument doc(
        "hello world. this is a test document. "
        "we will replace words and capitalize sentences!");

    std::cout << "Original:\n" << doc.content() << "\n\n";

    Wrapper replace_cmd(
        &doc, &TextDocument::replace_words,
        AnyMap{{"old", std::string("")}, {"new", std::string("")}});
    Wrapper search_cmd(&doc, &TextDocument::search_phrase,
                       AnyMap{{"phrase", std::string("")}});
    Wrapper cap_words_cmd(&doc, &TextDocument::capitalize_words, AnyMap{});
    Wrapper cap_sent_cmd(&doc, &TextDocument::capitalize_sentences, AnyMap{});
    Wrapper number_cmd(&doc, &TextDocument::number_sentences,
                       AnyMap{{"start", 1}});

    Engine doc_engine;
    doc_engine.register_command(&replace_cmd, "replace");
    doc_engine.register_command(&search_cmd, "search");
    doc_engine.register_command(&cap_words_cmd, "capitalize_words");
    doc_engine.register_command(&cap_sent_cmd, "capitalize_sentences");
    doc_engine.register_command(&number_cmd, "number_sentences");

    std::cout << "Search 'test': "
              << doc_engine.execute("search", {{"phrase", std::string("test")}})
              << "\n\n";

    std::cout << "After replace 'world' -> 'universe':\n"
              << doc_engine.execute("replace", {{"old", std::string("world")},
                                                {"new", std::string("universe")}})
              << "\n\n";

    std::cout << "After capitalize_sentences:\n"
              << doc_engine.execute("capitalize_sentences") << "\n\n";

    std::cout << "After capitalize_words:\n"
              << doc_engine.execute("capitalize_words") << "\n\n";

    std::cout << "After number_sentences (start=1):\n"
              << doc_engine.execute("number_sentences", {{"start", 1}})
              << "\n\n";

    std::cout << "After replace 'Test' -> 'Demo':\n"
              << doc_engine.execute("replace", {{"old", std::string("Test")},
                                                {"new", std::string("Demo")}})
              << '\n';
}

int main() {
    try {
        std::cout << "Hello Wrapper!\n";
        std::cout << "Hello Engine!\n";
        run_classic_demo();
        run_document_demo();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
