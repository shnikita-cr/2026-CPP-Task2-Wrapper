#pragma once

// Управляет вводом пользователя и связывает команды с текстовым документом.
class Application
{
public:
    // Запускает меню и возвращает код завершения программы.
    static int run();

private:
    // Включает UTF-8 в консоли Windows.
    static void configure_console();

    // Выводит список доступных операций.
    static void print_menu();
};
