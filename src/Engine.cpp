#include "Engine.h"

#include <stdexcept>

void Engine::register_command(const Wrapper* wrapper, const std::string& name) {
    if (wrapper == nullptr) {
        throw std::invalid_argument("Engine: wrapper is null");
    }
    const bool inserted = commands_.emplace(name, wrapper).second;
    if (!inserted) {
        throw std::invalid_argument("Engine: command '" + name + "' is already registered");
    }
}

std::string Engine::execute(const std::string& name, const ArgumentMap& overrides) const {
    const auto command = commands_.find(name);
    if (command == commands_.end()) {
        throw std::out_of_range("Engine: unknown command '" + name + "'");
    }
    return command->second->invoke(overrides).toString();
}
