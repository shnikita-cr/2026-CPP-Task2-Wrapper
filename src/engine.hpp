#pragma once

#include "wrapper.hpp"

#include <map>
#include <stdexcept>
#include <string>

class Engine {
public:
    using Args = Wrapper::Args;

    void register_command(Wrapper* wrapper, const std::string& name) {
        if (wrapper == nullptr) {
            throw std::invalid_argument("null wrapper");
        }
        commands_[name] = wrapper;
    }

    [[nodiscard]] std::string execute(const std::string& name, const Args& args = {}) const {
        const auto command = commands_.find(name);
        if (command == commands_.end()) {
            throw std::runtime_error("unknown" + name);
        }
        return command->second->invoke(args);
    }

private:
    std::map<std::string, Wrapper*> commands_;
};