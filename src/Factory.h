#pragma once

#include "IWrapper.h"
#include "Wrapper.h"

#include <memory>
#include <utility>

template <typename T, typename Method, typename... Args>
std::unique_ptr<IWrapper> makeWrapper(T* obj,
    Method&& method,
    Args&&... args) {
    return std::make_unique<Wrapper<T>>(
        obj,
        std::forward<Method>(method),
        std::forward<Args>(args)...);
}