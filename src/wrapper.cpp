#include "wrapper.h"

#include <algorithm>
#include <string>

Value Wrapper::call(const ArgList& overrides) const {
    std::vector<Value> values;
    values.reserve(defaults_.size());
    for (const auto& entry : defaults_) {
        values.push_back(entry.second);
    }
    for (const auto& entry : overrides) {
        const auto found = std::find_if(defaults_.begin(), defaults_.end(),
                                        [&entry](const auto& item) { return item.first == entry.first; });
        if (found == defaults_.end()) {
            throw std::invalid_argument("Unknown argument: " + entry.first);
        }
        values[static_cast<std::size_t>(found - defaults_.begin())] = entry.second;
    }
    return invoker_(values);
}
