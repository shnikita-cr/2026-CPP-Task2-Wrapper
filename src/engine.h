#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <stdexcept>
#include <string>
#include <utility>

#include "wrapper.h"

class Engine {
public:
    void register_command(const Wrapper* wrapper, std::string name) {
        if (wrapper == nullptr) {
            throw std::invalid_argument("Command wrapper cannot be null");
        }
        if (name.empty()) {
            throw std::invalid_argument("Command name cannot be empty");
        }
        commands_.insert_or_assign(std::move(name), wrapper);
    }

    std::string execute(const std::string& name,
                        const Arguments& overrides = {}) const {
        const auto command = commands_.find(name);
        if (command == commands_.end()) {
            throw std::out_of_range("Unknown command: " + name);
        }
        return command->second->execute(overrides);
    }

private:
    std::map<std::string, const Wrapper*> commands_;
};

#endif  // ENGINE_H
