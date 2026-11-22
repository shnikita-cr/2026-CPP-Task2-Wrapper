#pragma once

#include "Wrapper.h"

#include <map>
#include <string>

class Engine {
public:
    void register_command(Wrapper* wrapper, const std::string& alias);
    [[nodiscard]] std::string execute(const std::string& alias,
                                      const Arguments& overrides = {}) const;

private:
    // Wrapper должен существовать до тех пор, пока команда зарегистрирована.
    std::map<std::string, Wrapper*> commands_;
};
