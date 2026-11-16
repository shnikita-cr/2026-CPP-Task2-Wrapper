#include "wrapper.h"

#include <algorithm>
#include <set>

namespace {

void validate_defaults(const ArgumentList& defaults, std::size_t arity) {
    if (defaults.size() != arity) {
        throw std::invalid_argument("Wrapper: method takes " + std::to_string(arity) +
                                    " argument(s), but defaults describe " +
                                    std::to_string(defaults.size()));
    }
    std::set<std::string> seen;
    for (const auto& argument : defaults) {
        if (argument.first.empty()) {
            throw std::invalid_argument("Wrapper: argument name must not be empty");
        }
        if (!seen.insert(argument.first).second) {
            throw std::invalid_argument("Wrapper: duplicate argument name '" +
                                        argument.first + "'");
        }
    }
}

bool has_argument(const ArgumentList& defaults, const std::string& name) {
    return std::any_of(defaults.begin(), defaults.end(),
                       [&name](const auto& argument) { return argument.first == name; });
}

}  

Wrapper::Wrapper(Invoker invoker, ArgumentList defaults, std::size_t arity)
    : invoker_(std::move(invoker)), defaults_(std::move(defaults)) {
    validate_defaults(defaults_, arity);
}

std::string Wrapper::execute(const ArgumentMap& overrides) const {
    for (const auto& override_argument : overrides) {
        if (!has_argument(defaults_, override_argument.first)) {
            throw std::invalid_argument("Wrapper: unknown argument '" +
                                        override_argument.first + "'");
        }
    }

    std::vector<std::string> values;
    values.reserve(defaults_.size());
    for (const auto& [name, default_value] : defaults_) {
        const auto found = overrides.find(name);
        values.push_back(found == overrides.end() ? default_value : found->second);
    }
    return invoker_(values);
}

std::vector<std::string> Wrapper::argument_names() const {
    std::vector<std::string> names;
    names.reserve(defaults_.size());
    for (const auto& argument : defaults_) {
        names.push_back(argument.first);
    }
    return names;
}

const ArgumentList& Wrapper::defaults() const {
    return defaults_;
}
