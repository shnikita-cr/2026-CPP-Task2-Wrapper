#include "Engine.h"

#include <stdexcept>

void Engine::register_command(Command* command, const std::string& name) {
    if (command == nullptr) {
        throw std::invalid_argument("command must not be null");
    }
    if (name.empty()) {
        throw std::invalid_argument("command name must not be empty");
    }
    if (!commands_.emplace(name, command).second) {
        throw std::invalid_argument("command '" + name + "' is already registered");
    }
}

std::string Engine::execute(const std::string& name, const Arguments& arguments) const {
    const auto found = commands_.find(name);
    if (found == commands_.end()) {
        throw std::invalid_argument("unknown command '" + name + "'");
    }
    return found->second->call(arguments);
}
