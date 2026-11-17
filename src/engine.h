#ifndef ENGINE_H
#define ENGINE_H

#include "arg_value.h"
#include "wrapper.h"
#include <string>
#include <unordered_map>
#include <vector>

class Engine {
private:
    std::unordered_map<std::string, Wrapper*> commands_;

public:
    Engine() = default;

    void register_command(Wrapper* wrapper, const std::string& name);
    void register_command(const std::string& name, Wrapper* wrapper);
    void register_command(Wrapper& wrapper, const std::string& name);
    void register_command(const std::string& name, Wrapper& wrapper);

    bool unregister_command(const std::string& name);
    bool has_command(const std::string& name) const noexcept;

    std::string execute(const std::string& name, const ArgsMap& args = {}) const;

    template <typename T>
    T execute(const std::string& name, const ArgsMap& args = {}) const {
        std::string res = execute(name, args);
        return ArgValue(res).template as<T>();
    }

    std::vector<std::string> get_registered_commands() const;
    std::size_t size() const noexcept;
    void clear() noexcept;
};

#endif // ENGINE_H

