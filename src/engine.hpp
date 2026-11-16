#pragma once

#include "wrapper.hpp"

#include <string>
#include <unordered_map>

class Engine {
public:
    void register_command(const std::string& name, Wrapper wrapper);
    void register_command(Wrapper* wrapper, const std::string& name);
    [[nodiscard]] std::string execute(const std::string& name,
                                      const Wrapper::ArgMap& args = {}) const;
    [[nodiscard]] bool has_command(const std::string& name) const;

private:
    std::unordered_map<std::string, Wrapper> commands_;
};