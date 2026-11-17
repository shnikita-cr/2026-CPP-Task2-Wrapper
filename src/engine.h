#ifndef ENGINE_H
#define ENGINE_H

#include <functional>
#include <map>
#include <string>
#include <stdexcept>
#include "argument_dict.h"

class Engine {
public:
    using Command = std::function<std::string(const ArgumentDict&)>;

    template <typename TWrapper>
    void registerCommand(const TWrapper* wrapper, const std::string& name) {
        m_commands[name] = [wrapper](const ArgumentDict& overrides) {
            return wrapper->invoke(overrides);
            };
    }

    std::string execute(const std::string& name, const ArgumentDict& overrides = {}) const {
        auto it = m_commands.find(name);
        if (it == m_commands.end()) {
            throw std::runtime_error("Engine: unknown command: " + name);
        }
        return it->second(overrides);
    }

private:
    std::map<std::string, Command> m_commands;
};

#endif