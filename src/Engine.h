// Engine не владеет командами: обёртка должна жить, пока её вызывают.
#pragma once

#include <map>
#include <string>

#include "Wrapper.h"

class Engine {
public:
    // Бросает std::invalid_argument, если команда пустая или имя уже занято.
    void register_command(Command* command, const std::string& name);

    // Бросает std::invalid_argument, если команды нет.
    [[nodiscard]] std::string execute(const std::string& name, const Arguments& arguments = {}) const;

private:
    std::map<std::string, Command*> commands_;
};
