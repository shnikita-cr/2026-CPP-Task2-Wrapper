//
//  wrapper.h
//  Wrapper
//
//  Created by Богдан on 08.11.2026.
//

#ifndef WRAPPER_H
#define WRAPPER_H

#include "any_map.h"

#include <any>
#include <functional>
#include <memory>
#include <string>

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual std::any execute(const AnyMap& overrides) = 0;
};

class Wrapper : public ICommand {
public:
    Wrapper(std::function<std::any(const AnyMap&)> fn, AnyMap defaults = {})
        : fn_(std::move(fn)), defaults_(std::move(defaults)) {}

    template <typename T, typename R>
    Wrapper(T* obj, R (T::*method)(const AnyMap&), AnyMap defaults = {})
        : defaults_(std::move(defaults)) {
        fn_ = [obj, method](const AnyMap& args) -> std::any {
            return (obj->*method)(args);
        };
    }

    template <typename T>
    Wrapper(T* obj, void (T::*method)(const AnyMap&), AnyMap defaults = {})
        : defaults_(std::move(defaults)) {
        fn_ = [obj, method](const AnyMap& args) -> std::any {
            (obj->*method)(args);
            return std::any{};
        };
    }

    template <typename T, typename Method, typename Extractor>
    Wrapper(T* obj, Method method, AnyMap defaults, Extractor extractor)
        : defaults_(std::move(defaults)) {
        fn_ = [obj, method, extractor](const AnyMap& args) -> std::any {
            return extractor(obj, method, args);
        };
    }

    std::any execute(const AnyMap& overrides) override {
        AnyMap merged = merge_args(defaults_, overrides);
        return fn_(merged);
    }

    const AnyMap& defaults() const { return defaults_; }

private:
    std::function<std::any(const AnyMap&)> fn_;
    AnyMap defaults_;
};

template <typename T, typename Method, typename Extractor>
std::unique_ptr<Wrapper> make_wrapper(T* obj, Method method, AnyMap defaults,
                                      Extractor extractor) {
    return std::make_unique<Wrapper>(obj, method, std::move(defaults),
                                     std::move(extractor));
}

#endif  // WRAPPER_H
