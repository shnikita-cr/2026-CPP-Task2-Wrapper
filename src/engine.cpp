#include "engine.hpp"

#include <stdexcept>
#include <string>

void Engine::register_command(Wrapper* wrapper, const std::string& name) {
    if (wrapper == nullptr) {
        throw std::runtime_error("cannot register a null wrapper");
    }
    if (name.empty()) {
        throw std::runtime_error("command name must not be empty");
    }
    commands_[name] = wrapper;
}

std::string Engine::execute(const std::string& name, const ArgMap& args) const {
    const auto command = commands_.find(name);
    if (command == commands_.end()) {
        throw std::runtime_error("unknown command: " + name);
    }
    return command->second->execute(args);
}
