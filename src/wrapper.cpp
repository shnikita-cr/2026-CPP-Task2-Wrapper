#include "wrapper.h"

void Wrapper::check_defaults(std::size_t arity) const {
    if (defaults_.size() != arity) {
        throw std::invalid_argument("Method takes " + std::to_string(arity) +
                                    " argument(s), but " + std::to_string(defaults_.size()) +
                                    " defaults were given");
    }
    for (std::size_t first = 0; first < defaults_.size(); ++first) {
        if (defaults_[first].first.empty()) {
            throw std::invalid_argument("Argument name must not be empty");
        }
        for (std::size_t second = first + 1; second < defaults_.size(); ++second) {
            if (defaults_[first].first == defaults_[second].first) {
                throw std::invalid_argument("Duplicate argument name: " + defaults_[first].first);
            }
        }
    }
}

std::size_t Wrapper::position_of(const std::string& name) const {
    for (std::size_t index = 0; index < defaults_.size(); ++index) {
        if (defaults_[index].first == name) {
            return index;
        }
    }
    throw std::invalid_argument("Unknown argument: " + name);
}

TextList Wrapper::texts_from_defaults() const {
    TextList texts;
    texts.reserve(defaults_.size());
    for (const auto& [name, value] : defaults_) {
        texts.emplace_back(name, value.text());
    }
    return texts;
}

std::string Wrapper::execute(const ArgumentList& overrides) const {
    TextList arguments = texts_from_defaults();
    std::vector<bool> replaced(arguments.size(), false);
    for (const auto& [name, value] : overrides) {
        const std::size_t position = position_of(name);
        if (replaced[position]) {
            throw std::invalid_argument("Duplicate argument: " + name);
        }
        replaced[position] = true;
        arguments[position].second = value.text();
    }
    return invoker_(arguments);
}
