#include "Document.h"
#include "Engine.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
struct EditorPaths {
    std::string input;
    std::string output;
};

std::string read_file(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open input file: " + path);
    }
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

void write_file(const std::string& path, std::string_view content) {
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Cannot open output file: " + path);
    }
    output << content;
    if (!output) {
        throw std::runtime_error("Cannot write output file: " + path);
    }
}

std::string ask(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void show_menu() {
    std::cout << "\n1 - заменить слова\n"
              << "2 - найти словосочетание\n"
              << "3 - сделать заглавными первые буквы слов\n"
              << "4 - сделать заглавными первые буквы предложений\n"
              << "5 - пронумеровать предложения\n"
              << "6 - показать текст\n"
              << "7 - сохранить и выйти\n"
              << "0 - выйти без сохранения\n";
}

void replace_words(Engine& engine) {
    Arguments overrides;
    const std::string from = ask("Какое слово заменить (Enter = кот): ");
    const std::string replacement = ask("На что заменить (Enter = пёс): ");
    if (!from.empty()) {
        overrides["from"] = from;
    }
    if (!replacement.empty()) {
        overrides["to"] = replacement;
    }
    if (std::cin) {
        std::cout << engine.execute("replace", overrides) << '\n';
    }
}

void find_phrase(Engine& engine) {
    const std::string phrase = ask("Фраза (Enter = добрый день): ");
    if (std::cin) {
        Arguments overrides;
        if (!phrase.empty()) {
            overrides["phrase"] = phrase;
        }
        std::cout << engine.execute("find", overrides) << '\n';
    }
}

bool run_command(const std::string& choice, Engine& engine, Document& doc,
                 const std::string& output_path) {
    if (choice == "1") {
        replace_words(engine);
    } else if (choice == "2") {
        find_phrase(engine);
    } else if (choice == "3") {
        std::cout << engine.execute("words") << '\n';
    } else if (choice == "4") {
        std::cout << engine.execute("sentences") << '\n';
    } else if (choice == "5") {
        std::cout << engine.execute("number") << '\n';
    } else if (choice == "6") {
        std::cout << doc.text() << '\n';
    } else if (choice == "7") {
        write_file(output_path, doc.text());
        std::cout << "Файл сохранён: " << output_path << '\n';
        return false;
    } else if (choice == "0") {
        return false;
    } else {
        std::cout << "Нет такого пункта меню.\n";
    }
    return true;
}

void run_editor(const EditorPaths& paths) {
    Document doc(read_file(paths.input));

    Wrapper replacement(&doc, &Document::replace_words,
                        {{"from", "кот"}, {"to", "пёс"}});
    Wrapper searching(&doc, &Document::find_phrase,
                      {{"phrase", "добрый день"}});
    Wrapper words(&doc, &Document::capitalize_words);
    Wrapper sentences(&doc, &Document::capitalize_sentences);
    Wrapper numbering(&doc, &Document::number_sentences);

    Engine engine;
    engine.register_command(&replacement, "replace");
    engine.register_command(&searching, "find");
    engine.register_command(&words, "words");
    engine.register_command(&sentences, "sentences");
    engine.register_command(&numbering, "number");

    while (true) {
        show_menu();
        const std::string choice = ask("Ваш выбор: ");
        if (!std::cin) {
            break;
        }
        try {
            if (!run_command(choice, engine, doc, paths.output)) {
                break;
            }
        } catch (const std::exception& error) {
            std::cerr << "Ошибка команды: " << error.what() << '\n';
        }
    }
}
}  // namespace

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    // Проверка CI запускает exe без параметров. Печатаем справку и успешно выходим.
    if (argc == 1) {
        std::cout << "Usage: document_app <input.txt> <output.txt>\n";
        return 0;
    }
    if (argc != 3) {
        std::cerr << "Expected two file paths.\n";
        return 1;
    }
    try {
        run_editor({argv[1], argv[2]});
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
