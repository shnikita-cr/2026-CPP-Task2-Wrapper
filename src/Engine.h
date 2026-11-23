#pragma once

#include <functional>
#include <map>
#include <string>
#include <type_traits>

#include "ArgsMap.h"
#include "Convert.h"

class Engine {
private:
    using Command = std::function<std::string(const ArgsMap&)>;

    std::map<std::string, Command> commands;

public:
    template<typename W>
    void register_command(W* wrapper, const std::string& name) {
        commands.insert_or_assign(name, [wrapper](const ArgsMap& args) -> std::string {
                using Result = decltype(wrapper->invoke(args));

                if constexpr (std::is_void_v<Result>) {
                    wrapper->invoke(args);
                    return {};
                }
                else {
                    return resultToString(wrapper->invoke(args));
                }
            }
        );
    }

    std::string execute(const std::string& name, const ArgsMap& args) const {
        return commands.at(name)(args);
    }
};