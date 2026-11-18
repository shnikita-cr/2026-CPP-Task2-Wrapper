#ifndef ENGINE_H
#define ENGINE_H

#include "wrapper.h"

#include <stdexcept>
#include <string>
#include <unordered_map>

class Engine {
public:
    void register_command(const Wrapper* wrapper, const std::string& alias) {
        if (wrapper == nullptr) {
            throw std::invalid_argument("Cannot register a null Wrapper");
        }
        if (alias.empty()) {
            throw std::invalid_argument("Command alias cannot be empty");
        }
        // Engine keeps its own Wrapper copy. The wrapped object remains non-owning.
        const auto inserted = commands_.emplace(alias, *wrapper);
        if (!inserted.second) {
            throw std::invalid_argument("Command alias is already registered: " + alias);
        }
    }

    [[nodiscard]] std::string execute(const std::string& alias,
                                      const ArgumentMap& arguments = {}) const {
        const auto command = commands_.find(alias);
        if (command == commands_.end()) {
            throw std::out_of_range("Unknown command: " + alias);
        }
        return command->second.execute(arguments);
    }

private:
    std::unordered_map<std::string, Wrapper> commands_;
};

#endif
