#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Engine.h"
#include "TextEdit.h"
#include "Wrapper.h"

class Subject {
 public:
  [[nodiscard]] int func(int arg1, int arg2) const { return arg1 + arg2 + val; }

 private:
  int val{0};
};

int main() {
  try {
    Subject subj;
    Wrapper wrapper(&subj, &Subject::func, {{"arg1", 0}, {"arg2", 0}});
    Engine engine;
    engine.register_comand(&wrapper, "comand_name");
    constexpr int arg1_val = 4;
    constexpr int arg2_val = 5;
    std::cout << engine.execute("comand_name",
                                {{"arg1", arg1_val}, {"arg2", arg2_val}})
              << "\n\n";

    TextEdit editor;
    std::string sample_text =
        "Nauka_programmirovania_Zadanie_2 . there is a certain class and its "
        "instance. it is necessary to write a class with a method that can "
        "encapsulate this instance. Task 2.";

    auto replace_cmd =
        make_wrapper(&editor, &TextEdit::replace_words,
                     std::vector<std::pair<std::string, std::string>>{
                         {"text", sample_text},
                         {"old_word", "class"},
                         {"new_word", "object"}});

    Wrapper find_cmd(&editor, &TextEdit::find_phrase,
                     {{"text", sample_text}, {"phrase", "write a class"}});

    Wrapper cap_words_cmd(&editor, &TextEdit::capitalize_words,
                          {{"text", sample_text}});

    Wrapper cap_sent_cmd(&editor, &TextEdit::capitalize_sentences,
                         {{"text", sample_text}});

    Wrapper num_sent_cmd(&editor, &TextEdit::number_sentences,
                         {{"text", sample_text}});

    engine.register_comand(&replace_cmd, "replace");
    engine.register_comand(&find_cmd, "find");
    engine.register_comand(&cap_words_cmd, "cap_words");
    engine.register_comand(&cap_sent_cmd, "cap_sentences");
    engine.register_comand(&num_sent_cmd, "num_sentences");

    std::cout << "Original:\n" << sample_text << "\n\n";

    std::cout
        << "1)Replacing words in the text. (default 'class' -> 'object'):\n"
        << engine.execute("replace") << "\n\n";

    std::cout
        << "Replacing words in the text. (override 'instance' -> 'entity'):\n"
        << engine.execute("replace",
                          {{"old_word", "instance"}, {"new_word", "entity"}})
        << "\n\n";

    std::cout << "2)Search for word combinations (returns status: found / not "
                 "found). ('write a class'): "
              << engine.execute("find") << "\n";

    std::cout << "Search for word combinations (returns status: found / not "
                 "found). ('C++'): "
              << engine.execute("find", {{"phrase", "C++"}}) << "\n\n";

    std::cout << "3)Changing the first letters of words to uppercase "
                 "(capitalizing each word):\n"
              << engine.execute("cap_words") << "\n\n";

    std::string cap_text = engine.execute("cap_sentences");
    std::cout
        << "4)Changing the first letter of the sentence to a capital letter:\n"
        << cap_text << "\n\n";

    std::cout << "5)Adding sequential numbering to each sentence (with each "
                 "sentence starting on a new line):\n"
              << engine.execute("num_sentences", {{"text", cap_text}}) << "\n";

  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    return 1;
  }

  return 0;
}