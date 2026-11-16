#include "engine.hpp"

#include <stdexcept>
#include <utility>

void Engine::register_command(const std::string& name, Wrapper wrapper) {
    commands_.insert_or_assign(name, std::move(wrapper));
}

void Engine::register_command(Wrapper* wrapper, const std::string& name) {
    if (wrapper == nullptr) {
        throw std::invalid_argument("Wrapper pointer is null");
    }
    commands_.insert_or_assign(name, *wrapper);
}

std::string Engine::execute(const std::string& name,
                            const Wrapper::ArgMap& args) const {
    const auto command_it = commands_.find(name);
    if (command_it == commands_.end()) {
        throw std::runtime_error("Unknown command: " + name);
    }
    return command_it->second.invoke(args);
}

bool Engine::has_command(const std::string& name) const {
    return commands_.find(name) != commands_.end();
}