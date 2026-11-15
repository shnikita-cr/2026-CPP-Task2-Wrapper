#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>

#include "ArgumentMap.h"
#include "Wrapper.h"

class Engine {
public:
    void register_command(const Wrapper* wrapper, const std::string& name);

    [[nodiscard]] std::string execute(const std::string& name, const ArgumentMap& overrides = {}) const;

private:
    std::map<std::string, const Wrapper*> commands_;
};

#endif
