#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <stdexcept>
#include <string>

#include "Wrapper.h"

class Engine {
 public:
  template <typename ArgType, typename ReturnType>
  void register_comand(const ComandBase<ArgType, ReturnType>* comand,
                       const std::string& comand_name) {
    if (comand == nullptr) {
      throw std::invalid_argument("Com ptr cant 0");
    }
    comands_[comand_name] = comand;
  }

  std::string execute(const std::string& comand_name) const {
    return execute_typed<std::string, std::string>(comand_name, {});
  }

  std::string execute(const std::string& comand_name,
                      const std::map<std::string, std::string>& args) const {
    return execute_typed<std::string, std::string>(comand_name, args);
  }

  int execute(const std::string& comand_name,
              const std::map<std::string, int>& args) const {
    return execute_typed<int, int>(comand_name, args);
  }

 private:
  std::map<std::string, const AnyComand*> comands_;

  template <typename ArgType, typename ReturnType>
  ReturnType execute_typed(const std::string& comand_name,
                           const std::map<std::string, ArgType>& args) const {
    auto iter = comands_.find(comand_name);
    if (iter == comands_.end()) {
      throw std::runtime_error("Com not found: " + comand_name);
    }

    const auto* typed_cmd =
        dynamic_cast<const ComandBase<ArgType, ReturnType>*>(iter->second);
    if (typed_cmd == nullptr) {
      throw std::runtime_error("Invalid arg " + comand_name);
    }

    return typed_cmd->execute(args);
  }
};

#endif