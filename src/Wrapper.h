#pragma once

#include "IWrapper.h"

#include <functional>
#include <utility>

template <typename T>
class Wrapper : public IWrapper {
public:
    using MethodType = std::function<std::string(T*, const Arguments&)>;

    Wrapper(T* obj, MethodType method, Arguments defaultArgs)
        : m_obj(obj)
        , m_method(std::move(method))
        , m_defaultArgs(std::move(defaultArgs))
    {
    }

    Wrapper(Wrapper&& other) noexcept = default;
    Wrapper& operator=(Wrapper&& other) noexcept = default;

    Wrapper(const Wrapper&) = delete;
    Wrapper& operator=(const Wrapper&) = delete;

    ~Wrapper() override = default;

    std::string execute(const Arguments& overrideArgs) override {
        Arguments finalArgs = m_defaultArgs;
        for (const auto& pair : overrideArgs) {
            finalArgs[pair.first] = pair.second;
        }
        return m_method(m_obj, finalArgs);
    }

private:
    T* m_obj;
    MethodType m_method;
    Arguments m_defaultArgs;
};