#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <vector>

#include "wrapper.h"

class Engine {
public:

    void register_command(Wrapper* wrapper, const std::string& command_name);
    [[nodiscard]] std::string execute(const std::string& command_name,
                                      const ArgumentMap& arguments = {}) const;

    [[nodiscard]] bool has_command(const std::string& command_name) const;
    [[nodiscard]] std::vector<std::string> command_names() const;

private:
    std::map<std::string, Wrapper*> commands_;
};

#endif  
