#pragma once

#include <unordered_map>
#include <string>
#include <any>
#include <utility>
#include <vector>
#include <memory>

using ArgDict = std::unordered_map<std::string, std::any>;
using ArgVec = std::vector<std::pair<std::string, std::any>>;

class BaseWrapper {
public:
    virtual std::any execute(const ArgDict& arguments) = 0;

    virtual ~BaseWrapper() = default;
};

template <typename MethodPtr> 
class Wrapper;

template <typename Obj, typename Ret, typename ...Args>
class Wrapper<Ret (Obj::*)(Args...)> : public BaseWrapper {
    Obj* subj;
    Ret (Obj::*method)(Args...);
    ArgVec Vdefaults;

public:
    Wrapper(Obj* sss, Ret (Obj::*mmm)(Args...), ArgVec def)
     : subj(sss), method(mmm), Vdefaults(std::move(def)) {}

    std::any execute(const ArgDict& arguments) override {
        ArgDict merged;
        for (const auto& [name, value] : Vdefaults) {
            merged[name] = value;
        }

        for (const auto& [name, value] : arguments) {
            merged[name] = value;
        }
        return callfunc(merged, std::make_index_sequence<sizeof...(Args)>{});
    }

    template <size_t... I>
    std::any callfunc(const ArgDict& args, std::index_sequence<I...> seq) {
        (void)seq;
        if constexpr (std::is_void_v<Ret>) {
            (subj->*method)(std::any_cast<Args>(args.at(Vdefaults[I].first))...);
            return std::any{};
        } else {
            return std::any((subj->*method)(std::any_cast<Args>(args.at(Vdefaults[I].first))...));
        }
    }
};

template <typename Obj, typename Ret, typename... Args>
std::unique_ptr<BaseWrapper> make_wrapper(
    Obj* obj, Ret (Obj::*method)(Args...), const ArgVec& defaults) {
    return std::make_unique<Wrapper<Ret (Obj::*)(Args...)>>(obj, method, defaults);
}