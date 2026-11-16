#include "argument_set.h"

#include <algorithm>
#include <stdexcept>

ArgumentSet::ArgumentSet(std::initializer_list<Argument> arguments) {
    m_items.reserve(arguments.size());
    for (const Argument& argument : arguments) {
        if (contains(argument.name)) {
            throw std::invalid_argument("повторяющееся имя аргумента: " + argument.name);
        }
        m_items.push_back(argument);
    }
}

std::size_t ArgumentSet::size() const {
    return m_items.size();
}

bool ArgumentSet::empty() const {
    return m_items.empty();
}

bool ArgumentSet::contains(const std::string& name) const {
    return index_of(name) != m_items.size();
}

const Value& ArgumentSet::value_at(std::size_t index) const {
    return m_items.at(index).value;
}

ArgumentSet ArgumentSet::with_overrides(const ArgumentSet& overrides) const {
    ArgumentSet result = *this;
    for (const Argument& replacement : overrides.m_items) {
        const std::size_t index = result.index_of(replacement.name);
        if (index == result.m_items.size()) {
            throw std::invalid_argument("неизвестный аргумент: " + replacement.name);
        }
        result.m_items[index].value = replacement.value;
    }
    return result;
}

std::size_t ArgumentSet::index_of(const std::string& name) const {
    const auto found = std::find_if(m_items.begin(), m_items.end(),
                                    [&name](const Argument& argument) { return argument.name == name; });
    return static_cast<std::size_t>(found - m_items.begin());
}
