#include "engine.h"

#include <stdexcept>

void Engine::register_command(const ICommand* command, const std::string& alias)
{
    if (command == nullptr)
    {
        throw std::invalid_argument("Command must not be null");
    }
    if (alias.empty())
    {
        throw std::invalid_argument("Command alias must not be empty");
    }
    if (!commands_.emplace(alias, command).second)
    {
        throw std::invalid_argument("Command alias already registered: " + alias);
    }
}

std::string Engine::execute(const std::string& alias, const ArgumentMap& overrides) const
{
    const auto position = commands_.find(alias);
    if (position == commands_.end())
    {
        throw std::out_of_range("Unknown command alias: " + alias);
    }

    return position->second->execute(overrides);
}
