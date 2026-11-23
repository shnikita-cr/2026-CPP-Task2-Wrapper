#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Engine {
public:
    using Overrides = std::unordered_map<std::string, std::string>;
    using Command = std::function<std::string(const Overrides &)>;

    template<typename W>
    void register_command(W *wrapper, const std::string &alias) {
        commands_[alias] = wrapper->make_executor();
    }

    [[nodiscard]] std::string execute(const std::string &alias, const Overrides &ov = {}) {
        auto it = commands_.find(alias);
        if (it == commands_.end()) throw std::runtime_error("Unknown command: " + alias);
        return it->second(ov);
    }

private:
    std::unordered_map<std::string, Command> commands_;
};
