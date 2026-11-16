#pragma once

#include "Wrapper.h"
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Engine {
public:
    void register_command(Wrapper* wrapper, const std::string& command_name) {
        if (wrapper == nullptr) {
            throw std::invalid_argument("Wrapper pointer must not be null");
        }
        commands_[command_name] = wrapper;
    }
    std::string execute(const std::string& command_name, std::initializer_list<ArgPair> args = {}) {
        auto it = commands_.find(command_name);
        if (it == commands_.end()) {
            throw std::out_of_range("Command not registered: " + command_name);
        }
        ArgMap arg_map;
        for (const auto& [name, val] : args) {
            arg_map[name] = val;
        }
        const std::any result = it->second->execute(arg_map);
        if (!result.has_value()) {
            return "";
        }
        if (result.type() == typeid(std::string)) {
            return std::any_cast<std::string>(result);
        }
        if (result.type() == typeid(const char*)) {
            return std::string(std::any_cast<const char*>(result));
        }
        if (result.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(result));
        }
        if (result.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(result));
        }
        throw std::runtime_error("Unsupported return type for command: " + command_name);
    }
private:
    std::unordered_map<std::string, Wrapper*> commands_;
};
