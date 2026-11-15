#ifndef ARGUMENT_MAP_H
#define ARGUMENT_MAP_H

#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "Value.h"

class ArgumentMap {
public:
    using Entry = std::pair<std::string, Value>;

    ArgumentMap() = default;
    ArgumentMap(std::initializer_list<Entry> entries);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] const std::vector<Entry>& entries() const noexcept;
    [[nodiscard]] std::vector<Value> values() const;

    bool replace(const std::string& name, const Value& value);

private:
    std::vector<Entry> entries_;
};

#endif
