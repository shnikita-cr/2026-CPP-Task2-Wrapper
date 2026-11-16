#include "wrapper.hpp"

#include <utility>

Wrapper::Wrapper(Callable callable, ArgMap defaults)
    : callable_(std::move(callable)), defaults_(std::move(defaults)) {}

std::string Wrapper::invoke(const ArgMap& overrides) const {
    ArgMap merged = defaults_;
    for (const auto& [key, value] : overrides) {
        merged[key] = value;
    }
    return callable_(merged);
}

const Wrapper::ArgMap& Wrapper::defaults() const noexcept { return defaults_; }