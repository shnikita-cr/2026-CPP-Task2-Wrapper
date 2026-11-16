#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include "argument_set.h"
#include "wrapper.h"

// хранит команды Wrapper под строковыми псевдонимами и выполняет их по имени
class Engine {
public:
    void register_command(const Wrapper* wrapper, const std::string& name);

    [[nodiscard]] bool has_command(const std::string& name) const;

    [[nodiscard]] std::string execute(const std::string& name, const ArgumentSet& overrides = {}) const;

private:
    std::map<std::string, const Wrapper*> m_commands;
};

#endif
