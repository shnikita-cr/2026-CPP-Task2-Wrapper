#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>

#include "wrapper.h"

class Engine {
public:
    void register_command(Wrapper* wrapper, const std::string& name);
    [[nodiscard]] std::string execute(const std::string& name,
                                      const ArgumentList& arguments = {}) const;

private:
    std::map<std::string, Wrapper*> commands_;
};

#endif
