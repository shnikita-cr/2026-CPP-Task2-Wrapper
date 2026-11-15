#ifndef WRAPPER_ENGINE_H
#define WRAPPER_ENGINE_H

#include "arguments.h"
#include "command.h"

#include <string>
#include <unordered_map>

class Engine
{
  public:
    void register_command(const ICommand* command, const std::string& alias);

    [[nodiscard]] std::string execute(const std::string& alias,
                                      const ArgumentMap& overrides = {}) const;

  private:
    std::unordered_map<std::string, const ICommand*> commands_;
};

#endif // WRAPPER_ENGINE_H
