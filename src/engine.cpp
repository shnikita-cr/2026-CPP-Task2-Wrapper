#include "engine.h"

#include <stdexcept>

void Engine::register_command(Wrapper* wrapper, const std::string& command_name) {
    if (wrapper == nullptr) {
        throw std::invalid_argument("Engine: wrapper must not be nullptr");
    }
    if (command_name.empty()) {
        throw std::invalid_argument("Engine: command name must not be empty");
    }
    if (!commands_.emplace(command_name, wrapper).second) {
        throw std::invalid_argument("Engine: command '" + command_name +
                                    "' is already registered");
    }
}

std::string Engine::execute(const std::string& command_name,
                            const ArgumentMap& arguments) const {
    const auto found = commands_.find(command_name);
    if (found == commands_.end()) {
        throw std::out_of_range("Engine: unknown command '" + command_name + "'");
    }
    return found->second->execute(arguments);
}

bool Engine::has_command(const std::string& command_name) const {
    return commands_.find(command_name) != commands_.end();
}

std::vector<std::string> Engine::command_names() const {
    std::vector<std::string> names;
    names.reserve(commands_.size());
    for (const auto& command : commands_) {
        names.push_back(command.first);
    }
    return names;
}
