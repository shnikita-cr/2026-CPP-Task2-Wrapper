#pragma once

#include "Wrapper.hpp"

#include <any>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

class Engine {
public:
    void register_command(const Wrapper* command, const std::string& alias);

    template <typename Result = std::string>
    Result execute(const std::string& alias, const ParamMap& arguments = {}) const {
        const auto iter = registry_.find(alias);
        if (iter == registry_.end()) {
            throw std::out_of_range("Command not registered: " + alias);
        }

        std::any raw_result = iter->second->invoke(arguments);
        if constexpr (std::is_void_v<Result>) {
            return;
        } else {
            if (const auto* typed = std::any_cast<Result>(&raw_result); typed != nullptr) {
                return *typed;
            }
            throw std::runtime_error("Return type mismatch for command: " + alias);
        }
    }

private:
    std::unordered_map<std::string, const Wrapper*> registry_;
};
