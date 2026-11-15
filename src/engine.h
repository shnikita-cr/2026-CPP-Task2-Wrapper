#pragma once
#include "wrapper.h"

class Engine {
public:
    void register_command(const Wrapper* wrapper, const std::string& alias) {
        if (wrapper == nullptr || alias.empty()) {
            throw std::invalid_argument("Null wrapper or empty command name");
        }
        if (!commands_.emplace(alias, *wrapper).second) {
            throw std::invalid_argument("Duplicate command: " + alias);
        }
    }
    [[nodiscard]] std::string execute(const std::string& alias, const Arguments& arguments = {}) const {
        const auto command = commands_.find(alias);
        if (command == commands_.end()) {
            throw std::invalid_argument("Unknown command: " + alias);
        }
        return command->second.execute(arguments);
    }
private:
    std::map<std::string, Wrapper> commands_;
};
