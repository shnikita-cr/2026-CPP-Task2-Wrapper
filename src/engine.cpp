#include "engine.h"
#include <stdexcept>

void Engine::register_command(Wrapper* wrapper, const std::string& name) {
    if (wrapper == nullptr) {
        throw std::invalid_argument("Cannot register null wrapper");
    }
    if (name.empty()) {
        throw std::invalid_argument("Command name cannot be empty");
    }
    commands_[name] = wrapper;
}

void Engine::register_command(const std::string& name, Wrapper* wrapper) {
    register_command(wrapper, name);
}

void Engine::register_command(Wrapper& wrapper, const std::string& name) {
    register_command(&wrapper, name);
}

void Engine::register_command(const std::string& name, Wrapper& wrapper) {
    register_command(&wrapper, name);
}

bool Engine::unregister_command(const std::string& name) {
    return commands_.erase(name) > 0;
}

bool Engine::has_command(const std::string& name) const noexcept {
    return commands_.find(name) != commands_.end();
}

std::string Engine::execute(const std::string& name, const ArgsMap& args) const {
    auto found_iter = commands_.find(name);
    if (found_iter == commands_.end()) {
        throw std::runtime_error("Command not found: " + name);
    }
    if (found_iter->second == nullptr) {
        throw std::runtime_error("Registered wrapper for command '" + name + "' is null");
    }
    return found_iter->second->execute(args);
}

std::vector<std::string> Engine::get_registered_commands() const {
    std::vector<std::string> names;
    names.reserve(commands_.size());
    for (const auto& entry : commands_) {
        names.push_back(entry.first);
    }
    return names;
}

std::size_t Engine::size() const noexcept {
    return commands_.size();
}

void Engine::clear() noexcept {
    commands_.clear();
}
