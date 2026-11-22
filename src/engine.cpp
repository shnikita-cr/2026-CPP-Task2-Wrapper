#include "engine.h"

#include <stdexcept>

void Engine::register_command(Wrapper* wrapper, const std::string& name) {
    if (wrapper == nullptr) {
        throw std::invalid_argument("Wrapper must not be null");
    }
    if (name.empty()) {
        throw std::invalid_argument("Command name must not be empty");
    }
    if (!commands_.emplace(name, wrapper).second) {
        throw std::invalid_argument("Command already registered: " + name);
    }
}

std::string Engine::execute(const std::string& name, const ArgumentList& arguments) const {
    const auto found = commands_.find(name);
    if (found == commands_.end()) {
        throw std::invalid_argument("Unknown command: " + name);
    }
    return found->second->execute(arguments);
}