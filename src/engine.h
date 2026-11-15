//
//  engine.h
//  Wrapper
//
//  Created by Богдан on 08.11.2026.
//

#ifndef ENGINE_H
#define ENGINE_H

#include "wrapper.h"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Engine {
public:
    void register_command(ICommand* cmd, const std::string& name) {
        if (cmd == nullptr) {
            throw std::invalid_argument("Null command");
        }
        commands_[name] = cmd;
    }

    void register_command(std::unique_ptr<ICommand> cmd, const std::string& name) {
        owned_.push_back(std::move(cmd));
        commands_[name] = owned_.back().get();
    }

    std::any execute_any(const std::string& name, const AnyMap& overrides = {}) {
        auto it = commands_.find(name);
        if (it == commands_.end()) {
            throw std::runtime_error("Unknown command: " + name);
        }
        return it->second->execute(overrides);
    }

    std::string execute(const std::string& name, const AnyMap& overrides = {}) {
        std::any result = execute_any(name, overrides);
        if (!result.has_value()) {
            return "";
        }
        if (auto* s = std::any_cast<std::string>(&result)) {
            return *s;
        }
        if (auto* cs = std::any_cast<const char*>(&result)) {
            return (*cs != nullptr) ? *cs : "";
        }
        if (auto* i = std::any_cast<int>(&result)) {
            return std::to_string(*i);
        }
        if (auto* l = std::any_cast<long>(&result)) {
            return std::to_string(*l);
        }
        if (auto* d = std::any_cast<double>(&result)) {
            std::ostringstream oss;
            oss << *d;
            return oss.str();
        }
        if (auto* b = std::any_cast<bool>(&result)) {
            return *b ? "true" : "false";
        }
        return "[non-string result]";
    }

    bool has_command(const std::string& name) const {
        return commands_.count(name) > 0;
    }

private:
    std::unordered_map<std::string, ICommand*> commands_;
    std::vector<std::unique_ptr<ICommand>> owned_;
};

#endif  // ENGINE_H
