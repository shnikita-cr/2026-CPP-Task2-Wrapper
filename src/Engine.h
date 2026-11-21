
#pragma once
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "Wrapper.h"


class Engine {
public:

    // Register a command using a Wrapper reference
    template <typename Return_t>
    void register_command(const Wrapper<Return_t>& wrapper, const std::string& name) {

        if (name.empty()) {
            throw std::invalid_argument("Command name must not be empty");
        }

        // Check that the return of wrapper is convertable to string
        static_assert(
            std::is_convertible_v<std::string, Return_t>,
            "Command result must be constructible as std::string"
        );

        // Give a copy of wrapper and create function to execute wrapper
        Command_t command = [saved_wrapper = wrapper](const Arguments_t& arguments) -> std::string {
            return std::string(saved_wrapper.execute(arguments));
        };
        // Move command function to storage
        auto result = all_commands.try_emplace(name, std::move(command));

        if (!result.second) {
            throw std::invalid_argument("Command is already registered: " + name);
        }
    }


    // Register a command using a Wrapper pointer (from conditions)
    template <typename Return_t>
    void register_command(const Wrapper<Return_t>* wrapper, const std::string& name) {
        if (wrapper == nullptr) {
            throw std::invalid_argument("Cannot register a null Wrapper pointer");
        }
        register_command(*wrapper, name);
    }

    std::string execute(const std::string& name, const Arguments_t& arguments = {}) const {
        auto command = all_commands.find(name);
        if (command == all_commands.end()) {
            throw std::out_of_range("Unknown command: " + name);
        }
        return command->second(arguments);
    }


private:
    // Type for all registerd commands
    using Command_t = std::function<std::string(const Arguments_t&)>;
    std::unordered_map<std::string, Command_t> all_commands;
};
