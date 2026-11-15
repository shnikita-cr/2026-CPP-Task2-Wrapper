#include "Wrapper.h"

Value Wrapper::invoke(const ArgumentMap& overrides) const {
    ArgumentMap arguments = defaults_;
    for (const ArgumentMap::Entry& entry : overrides.entries()) {
        if (!arguments.replace(entry.first, entry.second)) {
            throw std::invalid_argument("Wrapper: unknown argument '" + entry.first + "'");
        }
    }
    return invoker_(arguments.values());
}
