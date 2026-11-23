#pragma once
#include <functional>
#include <map>
#include <any>
#include <string>

class WrapperBase {
public:
    virtual ~WrapperBase() = default;
    virtual std::string invoke(const std::map<std::string, std::any>& args) = 0;
};

template <typename T>
class Wrapper : public WrapperBase {
public:
    using FuncType = std::function<std::string(T*, const std::map<std::string, std::any>&)>;

    Wrapper(T* obj, FuncType f, const std::map<std::string, std::any>& def = {})
        : obj_(obj), func_(f), defaults_(def) {
    }

    std::string invoke(const std::map<std::string, std::any>& args) override {
        std::map<std::string, std::any> merged = defaults_;
        for (const auto& pair : args) {
            merged[pair.first] = pair.second;
        }
        return func_(obj_, merged);
    }

private:
    T* obj_;
    FuncType func_;
    std::map<std::string, std::any> defaults_;
};