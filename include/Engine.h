#ifndef ENGINE_H
#define ENGINE_H

#include "Wrapper.h"

#include <map>
#include <stdexcept>
#include <string>

class Engine {
public:
    void registerCommand(
        const std::string& alias,
        Wrapper& wrapper
    ) {
        commands_[alias] = &wrapper;
    }

    std::string execute(
        const std::string& alias,
        const Arguments& arguments = {}
    ) const {
        const auto command = commands_.find(alias);

        if (command == commands_.end()) {
            throw std::out_of_range(
                "Command not found: " + alias
            );
        }

        return command->second->execute(arguments);
    }

private:
    std::map<std::string, Wrapper*> commands_;
};

#endif // ENGINE_H
