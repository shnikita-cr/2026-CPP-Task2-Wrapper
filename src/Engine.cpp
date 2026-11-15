#include "Engine.h"

void Engine::register_command(std::unique_ptr<BaseWrapper> wrap, const std::string& commandName) {
    Wrappers[commandName] = std::move(wrap);

    std::cout << "Успешно зарегистрирована команда под названием: " << commandName << "\n";
}

std::any Engine::execute(const std::string& commandName, const ArgDict& arguments) {
    if (Wrappers.find(commandName) == Wrappers.end()) {
        throw std::runtime_error("Не найденно имя команды: " + commandName);
    }
    return Wrappers[commandName]->execute(arguments);
}