#include "engine.h"

#include <stdexcept>

void Engine::register_command(const Wrapper* wrapper, const std::string& name) {
    if (wrapper == nullptr) {
        throw std::invalid_argument("невозможно зарегистрировать пустую обертку как '" + name + "'");
    }
    if (!m_commands.emplace(name, wrapper).second) {
        throw std::invalid_argument("команда уже зарегистрирована: " + name);
    }
}

bool Engine::has_command(const std::string& name) const {
    return m_commands.count(name) != 0;
}

std::string Engine::execute(const std::string& name, const ArgumentSet& overrides) const {
    const auto found = m_commands.find(name);
    if (found == m_commands.end()) {
        throw std::out_of_range("неизвестная команда: " + name);
    }
    return found->second->call(overrides);
}
