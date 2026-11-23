#pragma once
#include <map>
#include <string>
#include <memory>
#include "wrapper.h"

class Engine {
public:
    void register_command(const std::string& name, std::shared_ptr<WrapperBase> wrapper) {
        commands_[name] = wrapper;
    }

    std::string execute(const std::string& name, const std::map<std::string, std::any>& args = {}) const {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return it->second->invoke(args);
        }
        return "Command not found";
    }

private:
    std::map<std::string, std::shared_ptr<WrapperBase>> commands_;
};