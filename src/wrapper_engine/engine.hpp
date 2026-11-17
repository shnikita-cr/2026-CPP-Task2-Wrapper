#pragma once

#include "wrapper_engine/types.hpp"

#include <string>
#include <unordered_map>

// Полное определение нужно только в файле реализации.
class Wrapper;

// Хранит команды под строковыми именами и запускает выбранную команду.
class Engine
{
public:
    // Связывает уникальный алиас с готовой оболочкой.
    void register_command(Wrapper* wrapper, const std::string& commandName);

    // Запускает команду с указанными аргументами или со всеми значениями по умолчанию.
    [[nodiscard]] std::string execute(const std::string& commandName, const ArgumentList& arguments = {}) const;

private:
    // Engine хранит адреса. Зарегистрированные Wrapper должны оставаться действительными по этим адресам.
    std::unordered_map<std::string, Wrapper*> _commands;
};
