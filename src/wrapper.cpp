#include "wrapper.h"

#include <stdexcept>

Wrapper::Wrapper(Invoker invoker, ArgumentSet defaults, std::size_t parameter_count)
    : m_defaults(std::move(defaults)), m_invoker(std::move(invoker)) {
    if (m_defaults.size() != parameter_count) {
        throw std::invalid_argument("the method takes " + std::to_string(parameter_count) + " parameter(s), but " +
                                    std::to_string(m_defaults.size()) + " default argument(s) were given");
    }
}

std::string Wrapper::call(const ArgumentSet& overrides) const {
    return m_invoker(m_defaults.with_overrides(overrides));
}