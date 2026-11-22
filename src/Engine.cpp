#include "Engine.h"

#include <stdexcept>

void Engine::register_command(Wrapper* wrapper, const std::string& alias) {
    if (wrapper == nullptr || alias.empty()) {
        throw std::invalid_argument("Invalid wrapper or command name");
    }
    if (!commands_.emplace(alias, wrapper).second) {
        throw std::invalid_argument("Command is already registered: " + alias);
    }
}

std::string Engine::execute(const std::string& alias,
                            const Arguments& overrides) const {
    const auto found = commands_.find(alias);
    if (found == commands_.end()) {
        throw std::out_of_range("Unknown command: " + alias);
    }
    return found->second->invoke(overrides);
}
