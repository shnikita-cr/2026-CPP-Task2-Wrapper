#pragma once

#include "argument.hpp"
#include "wrapper.hpp"

#include <map>
#include <string>

// Хранит зарегистрированные команды и выполняет их по строковому имени.
class Engine {
public:
    void register_command(Wrapper* wrapper, const std::string& name);

    [[nodiscard]] std::string execute(const std::string& name, const ArgMap& args = {}) const;

private:
    std::map<std::string, Wrapper*> commands_;
};
