#pragma once

#include "IWrapper.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

class Engine {
public:
    void register_command(std::unique_ptr<IWrapper> wrapper,
        const std::string& name) {
        m_commands[name] = std::move(wrapper);
    }

    std::string execute(const std::string& name,
        const Arguments& args = {}) {
        auto it = m_commands.find(name);
        if (it == m_commands.end()) {
            throw std::runtime_error("Command not found: " + name);
        }
        return it->second->execute(args);
    }

private:
    std::map<std::string, std::unique_ptr<IWrapper>> m_commands;
};