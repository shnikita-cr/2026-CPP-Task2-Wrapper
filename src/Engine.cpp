#include "Engine.hpp"

#include <stdexcept>

void Engine::register_command(const Wrapper* command, const std::string& alias) {
    if (command == nullptr) {
        throw std::invalid_argument("Cannot register a null Wrapper");
    }
    if (alias.empty()) {
        throw std::invalid_argument("Command alias must not be empty");
    }
    if (!registry_.emplace(alias, command).second) {
        throw std::invalid_argument("Command alias already registered: " + alias);
    }
}
