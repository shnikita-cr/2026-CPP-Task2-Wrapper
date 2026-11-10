#ifndef WRAPPER_COMMAND_H
#define WRAPPER_COMMAND_H

#include "arguments.h"

#include <string>

class ICommand
{
  public:
    virtual ~ICommand() = default;

    [[nodiscard]] virtual std::string execute(const ArgumentMap& overrides = {}) const = 0;
};

#endif // WRAPPER_COMMAND_H
