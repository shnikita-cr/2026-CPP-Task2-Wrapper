#include "DocumentProcessor.h"
#include "Engine.h"
#include "Wrapper.h"
#include <exception>
#include <iostream>
#include <string>

class Subject {
public:
    [[nodiscard]] int func(int arg1, int arg2) {
        last_result_ = arg1 + arg2;
        return last_result_;
    }
private:
    int last_result_ = 0;
};
int main() {
    try {
        constexpr int kDefaultArg = 0;
        constexpr int kFirstArg = 4;
        constexpr int kSecondArg = 5;
        Subject subj;
        Wrapper wrapper(&subj, &Subject::func, {{"arg1", kDefaultArg}, {"arg2", kDefaultArg}});
        Engine engine;
        engine.register_command(&wrapper, "command_name");
        std::cout << engine.execute("command_name", {{"arg1", kFirstArg}, {"arg2", kSecondArg}}) << '\n';
        DocumentProcessor doc("hello world. this is a simple test! c++ is great.");
        Wrapper w_replace(&doc, &DocumentProcessor::replace_word, {
            {"target", std::string("simple")},
            {"replacement", std::string("modern")}
        });
        Wrapper w_find(&doc, &DocumentProcessor::find_phrase, {
            {"phrase", std::string("test")}
        });
        Wrapper w_cap_words(&doc, &DocumentProcessor::capitalize_words, {});
        Wrapper w_cap_sentences(&doc, &DocumentProcessor::capitalize_sentences, {});
        Wrapper w_num_sentences(&doc, &DocumentProcessor::number_sentences, {});
        engine.register_command(&w_replace, "replace");
        engine.register_command(&w_find, "find");
        engine.register_command(&w_cap_words, "cap_words");
        engine.register_command(&w_cap_sentences, "cap_sentences");
        engine.register_command(&w_num_sentences, "num_sentences");
        std::cout << "Поиск: " << engine.execute("find", {{"phrase", "simple test"}}) << '\n';
        std::cout << "Замена: " << engine.execute("replace", {{"replacement", "powerful"}}) << '\n';
        std::cout << "Заглавные в предложениях: " << engine.execute("cap_sentences") << '\n';
        std::cout << "Нумерация:\n" << engine.execute("num_sentences") << '\n';
        std::cout << "Заглавные в словах:\n" << engine.execute("cap_words") << '\n';
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
