#pragma once

#include "Wrapper.h"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

class Engine {
private:
    std::map<std::string, WrapperBase*> m_commands;
    std::map<std::string, std::unique_ptr<WrapperBase>> m_owned_commands;

public:
    void register_command(WrapperBase* wrapper_ptr, const std::string& alias);

    void register_command(std::unique_ptr<WrapperBase> wrapper, const std::string& alias);

    std::string execute(const std::string& alias, const Arguments& arguments = {}) const;
};
