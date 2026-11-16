#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include "argument_set.h"
#include "engine.h"
#include "text_document.h"
#include "utf8.h"
#include "value.h"
#include "wrapper.h"

namespace {
int g_failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        ++g_failures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

template <typename Exception, typename Action>
bool throws(Action action) {
    try {
        action();
    } catch (const Exception&) {
        return true;
    }
    return false;
}

constexpr int kAnswer = 42;
constexpr int kFive = 5;
constexpr int kSeven = 7;
constexpr int kTwenty = 20;
constexpr double kHalfWay = 2.5;
constexpr double kFour = 4.0;
constexpr double kSevenAsDouble = 7.0;
constexpr double kFactor = 1.5;
constexpr std::size_t kLongWord = 5;

struct Point {
    int x;
    int y;
};

// проверка Wrapper
class Toolbox {
public:
    [[nodiscard]] std::string repeat(const std::string& word, int times) const {
        std::string result = m_prefix;
        for (int i = 0; i < times; ++i) {
            result += word;
        }
        return result;
    }

    [[nodiscard]] double scale(double value) const {
        return value * m_factor;
    }

    void remember(std::string note) {
        m_note = std::move(note);
    }

    [[nodiscard]] const std::string& note() const {
        return m_note;
    }

    [[nodiscard]] int area(const Point& point) const {
        return point.x * point.y * m_unit;
    }

    int next() noexcept {
        return ++m_counter;
    }

    [[nodiscard]] bool is_long(std::string&& word) const {
        return word.size() > kLongWord && m_counter >= 0;
    }

private:
    std::string m_prefix;
    std::string m_note;
    int m_unit = 1;
    int m_counter = 0;
    double m_factor = kFactor;
};

void test_value() {
    expect(Value(4).as<std::string>() == "4", "преобразование int в string");
    expect(Value("42").as<int>() == kAnswer, "преобразование string в int");
    expect(Value(kHalfWay).as<int>() == 2, "преобразование double в int");
    expect(Value(kSeven).as<double>() == kSevenAsDouble, "преобразование int в double");
    expect(Value("true").as<bool>(), "преобразование string в bool");
    expect(Value("literal").as<std::string>() == "literal", "строковый литерал сохраняется как std::string");
    expect(Value(std::string("text")).as<std::string>() == "text", "работа со std::string");
    expect(!Value("abc").try_as<int>().has_value(), "текст не является числом");
    expect(!Value("12abc").try_as<int>().has_value(), "лишние символы в конце отклоняются");
    expect(!Value().try_as<int>().has_value(), "пустое значение");
    expect(Value(Point{ 1, 2 }).as<Point>().y == 2, "работа с произвольным типом");
    expect(!Value(Point{ 1, 2 }).try_as<std::string>().has_value(), "произвольный тип не является строкой");
    expect(throws<ConversionError>([] { static_cast<void>(Value("abc").as<int>()); }), "as<> вызывает ConversionError");
}

void test_argument_set() {
    const ArgumentSet args{ {"first", 1}, {"second", 2} };
    expect(args.size() == 2 && !args.empty(), "проверка размера ArgumentSet");
    expect(args.contains("first") && !args.contains("third"), "проверка наличия аргумента");
    expect(args.value_at(1).as<int>() == 2, "порядок аргументов сохраняется");

    const ArgumentSet changed = args.with_overrides({ {"second", kTwenty} });
    expect(changed.value_at(0).as<int>() == 1 && changed.value_at(1).as<int>() == kTwenty, "переопределение аргумента по имени");
    expect(args.value_at(1).as<int>() == 2, "исходный набор аргументов не изменяется");

    expect(throws<std::invalid_argument>([&args] { static_cast<void>(args.with_overrides({ {"third", 3} })); }),
        "неизвестное имя аргумента при переопределении");
    expect(throws<std::invalid_argument>([] { const ArgumentSet duplicate{ {"a", 1}, {"a", 2} }; }), "повторяющиеся имена аргументов");
    expect(ArgumentSet{}.empty(), "пустой ArgumentSet");
}

void test_wrapper_basic() {
    Toolbox box;

    Wrapper repeat(&box, &Toolbox::repeat, { {"word", "ab"}, {"times", 2} });
    expect(repeat.call() == "abab", "используются значения по умолчанию");
    expect(repeat.call({ {"times", 3} }) == "ababab", "переопределение одного аргумента");
    expect(repeat.call({ {"word", "x"} }) == "xx", "переопределение другого аргумента");
    expect(repeat.call({ {"times", "4"}, {"word", "z"} }) == "zzzz", "преобразование текста в int");
    expect(repeat.call() == "abab", "значения по умолчанию не изменяются после вызова");

    Wrapper scale(&box, &Toolbox::scale, { {"value", 2} });
    expect(scale.call() == "3", "преобразование значения int по умолчанию в double");
    expect(scale.call({ {"value", kFour} }) == "6", "передача аргумента типа double");

    Wrapper next(&box, &Toolbox::next, {});
    expect(next.call() == "1" && next.call() == "2", "вызов noexcept-метода без аргументов");
}

void test_wrapper_special_signatures() {
    Toolbox box;

    Wrapper remember(&box, &Toolbox::remember, { {"note", "default"} });
    expect(remember.call({ {"note", "hello"} }).empty(), "void-метод возвращает пустую строку");
    expect(box.note() == "hello", "void-метод изменяет объект");

    Wrapper area(&box, &Toolbox::area, { {"point", Point{2, 3}} });
    expect(area.call() == "6", "пользовательский тип аргумента");
    expect(area.call({ {"point", Point{4, kFive}} }) == "20", "переопределение аргумента пользовательского типа");
    expect(throws<ConversionError>([&area] { static_cast<void>(area.call({ {"point", "oops"} })); }),
        "неверный тип аргумента");

    Wrapper is_long(&box, &Toolbox::is_long, { {"word", "abcdefgh"} });
    expect(is_long.call() == "true", "параметр типа rvalue reference и результат bool");
}

void test_wrapper_errors() {
    Toolbox box;
    expect(throws<std::invalid_argument>([&box] { const Wrapper bad(&box, &Toolbox::repeat, { {"word", "a"} }); }),
        "недостаточно значений по умолчанию");
    expect(throws<std::invalid_argument>([&box] { const Wrapper bad(&box, &Toolbox::next, { {"extra", 1} }); }),
        "слишком много значений по умолчанию");
    const Wrapper scale(&box, &Toolbox::scale, { {"value", 1} });
    expect(throws<std::invalid_argument>([&scale] { static_cast<void>(scale.call({ {"nope", 1} })); }),
        "неизвестный аргумент при вызове Wrapper");
}

void test_engine() {
    Toolbox box;
    Wrapper repeat(&box, &Toolbox::repeat, { {"word", "ab"}, {"times", 2} });
    Wrapper remember(&box, &Toolbox::remember, { {"note", "from engine"} });

    Engine engine;
    engine.register_command(&repeat, "repeat");
    engine.register_command(&remember, "remember");

    expect(engine.has_command("repeat") && !engine.has_command("other"), "проверка наличия команды");
    expect(engine.execute("repeat") == "abab", "выполнение команды со значениями по умолчанию");
    expect(engine.execute("repeat", { {"times", 1} }) == "ab", "выполнение команды с переопределением аргумента");
    static_cast<void>(engine.execute("remember"));
    expect(box.note() == "from engine", "команды работают с одним и тем же объектом");

    expect(throws<std::out_of_range>([&engine] { static_cast<void>(engine.execute("unknown")); }), "неизвестная команда");
    expect(throws<std::invalid_argument>([&engine, &repeat] { engine.register_command(&repeat, "repeat"); }),
        "повторяющееся имя команды");
    expect(throws<std::invalid_argument>([&engine] { engine.register_command(nullptr, "null"); }), "нулевой указатель на Wrapper");
    expect(throws<std::invalid_argument>([&engine] { static_cast<void>(engine.execute("repeat", { {"x", 1} })); }),
        "неизвестный аргумент при вызове через Engine");
    expect(!Engine{}.has_command("repeat"), "проверка пустого Engine");
}

void test_utf8() {
    const std::string mixed = "aé я \xF0\x9F\x98\x80";
    expect(utf8::encode(utf8::decode(mixed)) == mixed, "обратное преобразование UTF-8");
    expect(utf8::decode("a\xFF" "z") == U"a�z", "некорректный байт заменяется");
    expect(utf8::decode("\xD0") == U"�", "незавершённая последовательность заменяется");
    expect(utf8::to_upper(U'ё') == U'Ё' && utf8::to_lower(U'Ж') == U'ж', "изменение регистра кириллических символов");
    expect(utf8::to_upper(U'é') == U'É' && utf8::to_upper(U'÷') == U'÷', "изменение регистра символов Latin-1");
    expect(utf8::is_letter(U'я') && !utf8::is_letter(U'1') && !utf8::is_letter(U'-'), "проверка, является ли символ буквой");
}

void test_replace_words() {
    TextDocument cats("cat concatenate cat. A cat's cat!");
    expect(cats.replace_words("cat", "dog") == "dog concatenate dog. A cat's dog!", "заменяются только целые слова");

    TextDocument cars("a red car and a red carpet");
    expect(cars.replace_words("red car", "blue bike") == "a blue bike and a red carpet", "замена фразы");

    TextDocument russian("кот и котёнок, кот!");
    expect(russian.replace_words("кот", "пёс") == "пёс и котёнок, пёс!", "замена слов на кириллице");

    TextDocument same("nothing here");
    expect(same.replace_words("absent", "x") == "nothing here", "если совпадений нет, текст не изменяется");
    expect(throws<std::invalid_argument>([&same] { static_cast<void>(same.replace_words("", "x")); }),
        "пустой текст для замены");
}

void test_find_phrase() {
    const TextDocument doc("The quick\nbrown   fox. Jumps over!");
    expect(doc.find_phrase("quick brown"), "поиск фразы через перенос строки");
    expect(doc.find_phrase("QUICK  BROWN FOX"), "регистр и количество пробелов не учитываются");
    expect(doc.find_phrase("fox. jumps"), "поиск фразы со знаками препинания");
    expect(!doc.find_phrase("row"), "часть слова не считается фразой");
    expect(!doc.find_phrase("brown fo"), "часть последнего слова не считается совпадением");
    expect(!doc.find_phrase("quick fox"), "несоседние слова не образуют фразу");
    expect(!doc.find_phrase(""), "поиск пустой фразы");
    expect(!doc.find_phrase("   "), "поиск фразы из пробелов");

    const TextDocument russian("Съешь ещё этих мягких булок");
    expect(russian.find_phrase("ЕЩЁ ЭТИХ"), "поиск кириллической фразы без учёта регистра");
}

void test_capitalization() {
    TextDocument words("hello wORLD don't 3d well-known  x");
    expect(words.capitalize_words() == "Hello WORLD Don't 3d Well-Known  X", "заглавные буквы в начале слов");

    TextDocument russian_words("привет, мир ёлка");
    expect(russian_words.capitalize_words() == "Привет, Мир Ёлка", "заглавные буквы в начале кириллических слов");

    TextDocument sentences("hello world. how are you? fine!  3.14 is pi. \"quoted.\" next one... and more");
    expect(sentences.capitalize_sentences() ==
        "Hello world. How are you? Fine!  3.14 is pi. \"Quoted.\" Next one... And more",
        "заглавные буквы в начале предложений");

    TextDocument russian_sentences("привет. как дела? хорошо!");
    expect(russian_sentences.capitalize_sentences() == "Привет. Как дела? Хорошо!", "заглавные буквы в начале предложений на кириллице");
}

void test_numbering() {
    TextDocument doc("first one. second  one!\nthird? no end");
    expect(doc.number_sentences() == "1. first one.\n2. second one!\n3. third?\n4. no end", "нумерация предложений");

    TextDocument russian("привет. мир!");
    expect(russian.number_sentences() == "1. привет.\n2. мир!", "нумерация предложений на кириллице");

    TextDocument empty("");
    expect(empty.number_sentences().empty(), "пустой документ");
    TextDocument blank("  \n ");
    expect(blank.number_sentences().empty(), "документ, содержащий только пробелы");
}

void test_document_through_engine() {
    TextDocument doc("hello there. general kenobi!");
    Wrapper capital(&doc, &TextDocument::capitalize_sentences, {});
    Wrapper numbering(&doc, &TextDocument::number_sentences, {});
    Wrapper replace(&doc, &TextDocument::replace_words, { {"replaced", "hello"}, {"replacement", "hi"} });
    Wrapper find(&doc, &TextDocument::find_phrase, { {"phrase", "general kenobi"} });

    Engine engine;
    engine.register_command(&capital, "capitalize");
    engine.register_command(&numbering, "number");
    engine.register_command(&replace, "replace");
    engine.register_command(&find, "find");

    expect(engine.execute("find") == "true", "поиск фразы через Engine");
    expect(engine.execute("find", { {"phrase", "there general"} }) == "false", "поиск фразы через границу предложений");
    static_cast<void>(engine.execute("replace"));
    static_cast<void>(engine.execute("capitalize"));
    expect(engine.execute("number") == "1. Hi there.\n2. General kenobi!", "последовательное выполнение команд");
    expect(doc.text() == "1. Hi there.\n2. General kenobi!", "документ сохраняет результат обработки");
}

}

int main() {
    try {
        test_value();
        test_argument_set();
        test_wrapper_basic();
        test_wrapper_special_signatures();
        test_wrapper_errors();
        test_engine();
        test_utf8();
        test_replace_words();
        test_find_phrase();
        test_capitalization();
        test_numbering();
        test_document_through_engine();
    }
    catch (const std::exception& ex) {
        std::cerr << "Непредвиденная ошибка: " << ex.what() << '\n';
        return 1;
    }
    if (g_failures != 0) {
        std::cerr << "Количество не пройденных проверок: " << g_failures << '\n';
        return 1;
    }
    std::cout << "Все тесты пройдены успешно\n";
    return 0;
}