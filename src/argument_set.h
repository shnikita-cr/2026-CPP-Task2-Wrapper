#ifndef ARGUMENT_SET_H
#define ARGUMENT_SET_H

#include <cstddef>
#include <initializer_list>
#include <string>
#include <vector>

#include "value.h"

// один именованный аргумент
struct Argument {
    std::string name;
    Value value;
};

// словарь аргументов, который помнит порядок добавления
class ArgumentSet {
public:
    ArgumentSet() = default;

    ArgumentSet(std::initializer_list<Argument> arguments);

    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] bool contains(const std::string& name) const;

    // значение i-го аргумента
    [[nodiscard]] const Value& value_at(std::size_t index) const;

    // копия набора, в которой значения с совпадающими именами заменены на новые
    [[nodiscard]] ArgumentSet with_overrides(const ArgumentSet& overrides) const;

private:
    // индекс аргумента с таким именем или size(), если его нет
    [[nodiscard]] std::size_t index_of(const std::string& name) const;

    std::vector<Argument> m_items;
};

#endif