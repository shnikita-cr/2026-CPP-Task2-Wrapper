#include "wrapper_engine/application.hpp"

#include "wrapper_engine/engine.hpp"
#include "wrapper_engine/text_document.hpp"
#include "wrapper_engine/wrapper.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <utility>

#ifdef _WIN32
#include <Windows.h>
#endif

int Application::run()
{
    // Русский текст должен одинаково читаться и выводиться в Windows.
    configure_console();

    std::cout << "Введите текст документа одной строкой:\n> ";

    // getline() сохраняет пробелы внутри введённого текста.
    std::string initialText;
    std::getline(std::cin, initialText);

    // Пустой ввод запускает программу с готовым примером.
    if (initialText.empty())
    {
        initialText = "hello world. this is a text document! world is beautiful.";
        std::cout << "Использован демонстрационный текст:\n"
                  << initialText << "\n";
    }

    // Все зарегистрированные методы работают с одним документом.
    TextDocument document(std::move(initialText));

    // Для замены запоминаем два именованных строковых параметра.
    Wrapper replaceWrapper(&document, &TextDocument::replace_words, {{"old_word", std::string{}}, {"new_word", std::string{}}});

    // Метод поиска получает одну фразу и не изменяет документ.
    Wrapper findWrapper(&document, &TextDocument::find_phrase, {{"phrase", std::string{}}});

    // Следующие три метода не требуют аргументов.
    Wrapper capitalizeWordsWrapper(&document, &TextDocument::capitalize_words, {});

    Wrapper capitalizeSentencesWrapper(&document, &TextDocument::capitalize_sentences, {});

    Wrapper numberSentencesWrapper(&document, &TextDocument::number_sentences, {});

    // Алиасы отделяют меню от конкретных методов TextDocument.
    Engine engine;
    engine.register_command(&replaceWrapper, "replace");
    engine.register_command(&findWrapper, "find");
    engine.register_command(&capitalizeWordsWrapper, "capitalize_words");
    engine.register_command(&capitalizeSentencesWrapper, "capitalize_sentences");
    engine.register_command(&numberSentencesWrapper, "number_sentences");

    // Документ остаётся доступен для нескольких последовательных операций.
    while (true)
    {
        // Перед каждым выбором показываем актуальное состояние текста.
        std::cout << "\nТекущий документ:\n"
                  << document.text() << "\n\n";
        print_menu();

        std::string choice;
        // Ноль или закрытый поток ввода завершает приложение.
        if (!std::getline(std::cin, choice) || choice == "0")
        {
            return 0;
        }

        try
        {
            std::string result;

            if (choice == "1")
            {
                // Новые значения заменят оба аргумента Wrapper по умолчанию.
                std::string oldWord;
                std::string newWord;

                std::cout << "Какое слово заменить: ";
                std::getline(std::cin, oldWord);
                std::cout << "На какое слово заменить: ";
                std::getline(std::cin, newWord);

                result = engine.execute("replace", {{"old_word", oldWord}, {"new_word", newWord}});
            }
            else if (choice == "2")
            {
                // Поиск возвращает сообщение и не меняет текущий текст.
                std::string phrase;
                std::cout << "Введите искомую фразу: ";
                std::getline(std::cin, phrase);

                std::cout << engine.execute("find", {{"phrase", phrase}}) << "\n";
                continue;
            }
            else if (choice == "3")
            {
                // Команда без аргументов использует пустой ArgumentList.
                result = engine.execute("capitalize_words");
            }
            else if (choice == "4")
            {
                // Первая буква меняется только после конца предложения.
                result = engine.execute("capitalize_sentences");
            }
            else if (choice == "5")
            {
                // После нумерации каждое предложение начинается с новой строки.
                result = engine.execute("number_sentences");
            }
            else if (choice == "6")
            {
                // Новый текст записывается в тот же объект TextDocument.
                std::string newText;
                std::cout << "Введите новый текст: ";
                std::getline(std::cin, newText);
                document.set_text(std::move(newText));
                continue;
            }
            else
            {
                // Неизвестный ввод не прерывает основной цикл.
                std::cout << "Неизвестный пункт меню.\n";
                continue;
            }

            // У всех команд, изменяющих текст через Engine, общий вывод.
            std::cout << "\nРезультат:\n"
                      << result << "\n";
        }
        catch (const std::exception& exception)
        {
            // Ошибка одной команды не должна завершать приложение.
            std::cout << "Ошибка: " << exception.what() << "\n";
        }
    }
}

void Application::configure_console()
{
#ifdef _WIN32
    // Исходники и ввод пользователя обрабатываются в кодировке UTF-8.
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void Application::print_menu()
{
    // Номера пунктов остаются простыми для проверки всех команд вручную.
    std::cout
        << "1 - заменить слово\n"
        << "2 - найти словосочетание\n"
        << "3 - сделать первые буквы слов заглавными\n"
        << "4 - сделать первые буквы предложений заглавными\n"
        << "5 - пронумеровать предложения с новой строки\n"
        << "6 - ввести новый текст\n"
        << "0 - завершить программу\n"
        << "> ";
}
