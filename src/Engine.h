#ifndef ENGINE_H
#define ENGINE_H

#include "Wrapper.h"
#include <map>
#include <stdexcept>
#include <string>

class Engine {
public:
    Engine() = default;
    ~Engine() noexcept = default;

    Engine(const Engine&) = default;
    Engine& operator=(const Engine&) = default;
    Engine(Engine&&) noexcept = default;
    Engine& operator=(Engine&&) noexcept = default;

    void register_command(Wrapper* wrapper, const std::string& command_name) {
        if (wrapper == nullptr) {
            throw std::invalid_argument("Wrapper pointer cannot be null");
        }
        if (command_name.empty()) {
            throw std::invalid_argument("Command name cannot be empty");
        }
        commands_[command_name] = wrapper;
    }

    [[nodiscard]] std::string execute(const std::string& command_name,
                                      const Wrapper::ArgMap& args = {}) const {
        auto iter = commands_.find(command_name);
        if (iter == commands_.end()) {
            throw std::runtime_error("Command not found: " + command_name);
        }
        return iter->second->execute(args);
    }

private:
    std::map<std::string, Wrapper*> commands_;
};

#endif
