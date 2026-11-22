#pragma once

#include <map>
#include <memory>
#include <string>

using Arguments = std::map<std::string, std::string>;

class IWrapper {
public:
    virtual ~IWrapper() = default;
    virtual std::string execute(const Arguments& overrideArgs) = 0;
};