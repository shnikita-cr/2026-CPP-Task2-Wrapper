#include "wrapper_engine/engine.hpp"

#include "wrapper_engine/wrapper.hpp"

#include <stdexcept>

void Engine::register_command(Wrapper* wrapper, const std::string& commandName)
{
    // Engine хранит только адрес готового Wrapper.
    if (wrapper == nullptr)
    {
        throw std::invalid_argument("Cannot register a null wrapper.");
    }

    // Пустую строку невозможно использовать как осмысленный алиас.
    if (commandName.empty())
    {
        throw std::invalid_argument("A command name cannot be empty.");
    }

    // emplace() сразу сообщает, было ли имя свободно.
    const bool inserted = _commands.emplace(commandName, wrapper).second;
    if (!inserted)
    {
        throw std::invalid_argument("The command is already registered: " + commandName);
    }
}

std::string Engine::execute(const std::string& commandName, const ArgumentList& arguments) const
{
    // По алиасу получаем Wrapper с нужным объектом и методом.
    const auto iterator = _commands.find(commandName);
    if (iterator == _commands.end())
    {
        throw std::invalid_argument("Unknown command: " + commandName);
    }

    // Подготовку параметров и сам вызов выполняет Wrapper.
    return iterator->second->execute(arguments);
}
