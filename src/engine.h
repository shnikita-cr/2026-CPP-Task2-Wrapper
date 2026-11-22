#pragma once
#include <vector>
#include <any>
#include <unordered_map>
#include <functional>
#include <utility>
#include <stdexcept>
#include "wrapper.h"

using ArgMap = std::unordered_map<std::string, std::any>;
using ArgVec = std::vector<std::pair<std::string, std::any>>;

class Engine {
private:
    std::unordered_map<std::string, BaseWrapper*> commands;
public:
    void register_command(BaseWrapper* wrap, const std::string& command_name); 
    template <class T = std::string>
    T execute(const std::string& name, const ArgMap& arguments = {}) {
        auto it = commands.find(name);
        if (it == commands.end()) {
            throw std::runtime_error("Unknown command: " + name);
        }
        std::any result = it->second->execute(arguments);

        if constexpr (std::is_void_v<T>) {
            return;
        }
        else {
            return std::any_cast<T>(result);
        }
    }
};