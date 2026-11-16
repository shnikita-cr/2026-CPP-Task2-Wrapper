#pragma once

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "wrapper.h"

class Engine {
public:
    void register_command(std::unique_ptr<WrapperBase> wrapper,
        const std::string& name) {
        m_commands[name] = std::move(wrapper);
    }

    [[nodiscard]] std::string execute(
        const std::string& name,
        const std::map<std::string, std::string>& overrides = {}) {
        auto it = m_commands.find(name);
        if (it == m_commands.end()) {
            throw std::runtime_error("Unknown command: " + name);
        }
        return it->second->execute(overrides);
    }

private:
    std::map<std::string, std::unique_ptr<WrapperBase>> m_commands;
};