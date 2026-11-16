#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>

#include "value.h"
#include "wrapper.h"

// Хранит команды (Wrapper) по строковым алиасам и выполняет их.
class Engine {
public:
    void register_command(Wrapper* wrapper, const std::string& name);
    [[nodiscard]] Value execute(const std::string& name, const ArgList& args = {}) const;

private:
    std::map<std::string, Wrapper*> commands_;
};

#endif // ENGINE_H
