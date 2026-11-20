#include <cctype>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
using ArgMap = std::map<std::string, std::string>;

// 1. Базовый интерфейс команды для хранения в Engine
class WrapperBase {
public:
    virtual ~WrapperBase() = default;
    virtual std::string run(const ArgMap& override_args = {}) = 0;
};

// 2. Шаблонная обертка над объектом, его методом и аргументами по умолчанию
template <typename Subj>
class Wrapper : public WrapperBase {
public:
    using MethodPtr = std::string (Subj::*)(const ArgMap&);

    Wrapper(Subj* obj, MethodPtr method, const ArgMap& default_args)
        : obj_(obj), method_(method), default_args_(default_args) {}

    std::string run(const ArgMap& override_args = {}) override {
        ArgMap current_args = default_args_;
        for (const auto& [arg_name, val] : override_args) {
            current_args[arg_name] = val;
        }
        return (obj_->*method_)(current_args);
    }

private:
    Subj* obj_;
    MethodPtr method_;
    ArgMap default_args_;
};

// 3. Движок регистрации и выполнения команд по строковому имени
class Engine {
public:
    void register_command(std::unique_ptr<WrapperBase> cmd, const std::string& name) {
        commands_[name] = std::move(cmd);
    }

    std::string execute(const std::string& name, const ArgMap& args = {}) {
        auto it = commands_.find(name);
        if (it == commands_.end()) {
            return "[Error] Unknown command: " + name;
        }
        return it->second->run(args);
    }

private:
    std::map<std::string, std::unique_ptr<WrapperBase>> commands_;
};

// 4. Класс текстового документа с 5 операциями модификации текста
class TextDocument {
public:
    // Операция 1: Замена слов в тексте
    std::string replace_words(const ArgMap& args) {
        std::string text = get_arg(args, "text");
        std::string from = get_arg(args, "from");
        std::string to   = get_arg(args, "to");

        if (from.empty()) {
            return text;
        }

        size_t pos = 0;
        while ((pos = text.find(from, pos)) != std::string::npos) {
            text.replace(pos, from.size(), to);
            pos += to.size();
        }
        return text;
    }

    // Операция 2: Поиск словосочетаний (возвращает количество и позиции вхождений)
    std::string find_phrase(const ArgMap& args) {
        std::string text   = get_arg(args, "text");
        std::string phrase = get_arg(args, "phrase");

        if (phrase.empty()) {
            return "Empty phrase";
        }

        std::vector<size_t> positions;
        size_t pos = 0;
        while ((pos = text.find(phrase, pos)) != std::string::npos) {
            positions.push_back(pos);
            pos += phrase.size();
        }

        if (positions.empty()) {
            return "Phrase \"" + phrase + "\" not found.";
        }

        std::ostringstream oss;
        oss << "Found \"" << phrase << "\" " << positions.size() << " time(s) at index: ";
        for (size_t i = 0; i < positions.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << positions[i];
        }
        return oss.str();
    }

    // Операция 3: Заглавная первая буква у каждого слова
    std::string capitalize_words(const ArgMap& args) {
        std::string text = get_arg(args, "text");
        bool new_word = true;

        for (char& ch : text) {
            unsigned char uch = static_cast<unsigned char>(ch);
            if (std::isspace(uch) || std::ispunct(uch)) {
                new_word = true;
            } else if (new_word && std::isalpha(uch)) {
                ch = static_cast<char>(std::toupper(uch));
                new_word = false;
            } else {
                new_word = false;
            }
        }
        return text;
    }

    // Операция 4: Заглавная первая буква у каждого предложения
    std::string capitalize_sentences(const ArgMap& args) {
        std::string text = get_arg(args, "text");
        bool new_sentence = true;

        for (char& ch : text) {
            unsigned char uch = static_cast<unsigned char>(ch);
            if (ch == '.' || ch == '!' || ch == '?') {
                new_sentence = true;
            } else if (new_sentence && std::isalpha(uch)) {
                ch = static_cast<char>(std::toupper(uch));
                new_sentence = false;
            }
        }
        return text;
    }

    // Операция 5: Нумерация каждого предложения с новой строки
    std::string number_sentences(const ArgMap& args) {
        std::string text = get_arg(args, "text");
        std::ostringstream out;
        std::string current;
        int index = 1;

        auto flush_sentence = [&]() {
            size_t start = current.find_first_not_of(" \t\r\n");
            size_t end   = current.find_last_not_of(" \t\r\n");
            if (start != std::string::npos) {
                if (index > 1) out << '\n';
                out << index++ << ". " << current.substr(start, end - start + 1);
            }
            current.clear();
        };

        for (char ch : text) {
            current += ch;
            if (ch == '.' || ch == '!' || ch == '?') {
                flush_sentence();
            }
        }
        flush_sentence();
        return out.str();
    }

private:
    static std::string get_arg(const ArgMap& args, const std::string& key) {
        auto it = args.find(key);
        return (it != args.end()) ? it->second : "";
    }
};

int main() {
    TextDocument doc;
    Engine engine;

  std::ifstream file("text.txt");
    if (!file.is_open()) {
        std::cerr << "no text.txt\n";
    return 1;
}

    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string sample_text = buffer.str();

    // Регистрация всех 5 команд со значениями по умолчанию
    engine.register_command(
        std::make_unique<Wrapper<TextDocument>>(
            &doc, &TextDocument::replace_words,
            ArgMap{{"text", sample_text}, {"from", "c++"}, {"to", "modern C++"}}
        ),
        "replace"
    );

    engine.register_command(
        std::make_unique<Wrapper<TextDocument>>(
            &doc, &TextDocument::find_phrase,
            ArgMap{{"text", sample_text}, {"phrase", "c++"}}
        ),
        "find"
    );

    engine.register_command(
        std::make_unique<Wrapper<TextDocument>>(
            &doc, &TextDocument::capitalize_words,
            ArgMap{{"text", sample_text}}
        ),
        "cap_words"
    );

    engine.register_command(
        std::make_unique<Wrapper<TextDocument>>(
            &doc, &TextDocument::capitalize_sentences,
            ArgMap{{"text", sample_text}}
        ),
        "cap_sentences"
    );

    engine.register_command(
        std::make_unique<Wrapper<TextDocument>>(
            &doc, &TextDocument::number_sentences,
            ArgMap{{"text", sample_text}}
        ),
        "num_sentences"
    );

    // Демонстрация вызовов
    std::cout << "=== 1. Replace (default) ===\n"
              << engine.execute("replace") << "\n\n";

    std::cout << "=== 1b. Replace (override 'to') ===\n"
              << engine.execute("replace", {{"to", "C++17"}}) << "\n\n";

    std::cout << "=== 2. Find phrase ===\n"
              << engine.execute("find") << "\n\n";

    std::cout << "=== 3. Capitalize words ===\n"
              << engine.execute("cap_words") << "\n\n";

    std::cout << "=== 4. Capitalize sentences ===\n"
              << engine.execute("cap_sentences") << "\n\n";

    std::cout << "=== 5. Number sentences ===\n"
              << engine.execute("num_sentences") << "\n";

    return 0;
}