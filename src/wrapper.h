#pragma once
#include <string>
#include <initializer_list>
#include <memory>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <functional>

using ArgMap = std::unordered_map<std::string, std::any>;
using ArgVec = std::vector<std::pair<std::string, std::any>>;

class BaseWrapper {
public:
	virtual std::any execute(const ArgMap& arguments) = 0;
	virtual ~BaseWrapper() = default;
};

template <typename Class_t, typename Method_t, typename ... Args>

class Wrapper : public BaseWrapper {
public:
	Class_t* example;
	Method_t(Class_t::* func) (Args...);
	ArgVec default_;
	Wrapper(Class_t* exam, Method_t(Class_t::* func_n)(Args...), ArgVec defaults) : example(exam), func(func_n), default_(std::move(defaults)) {}
	std::any execute(const ArgMap& arguments) override {
		ArgMap new_args;
		for (auto& [name, value] : default_) {
			new_args[name] = value;
		}
		for (const auto& [name, value] : arguments) {
			new_args[name] = value;
		}
		return call_func(new_args, std::make_index_sequence<sizeof...(Args)> {});
	}
private:
	template <std::size_t... I>
	std::any call_func(const ArgMap& args, std::index_sequence<I...>) {
		if constexpr (std::is_void_v<Method_t>) {
			(example->*func)(std::any_cast<Args>(args.at(default_[I].first))...);
			return std::any{};
		}
		else {
			return (example->*func)(std::any_cast<Args>(args.at(default_[I].first))...);
		}
	}
};

template <typename Class_t, typename Method_t, typename ... Args>
std::unique_ptr<BaseWrapper> make_wrapper(Class_t* obj, Method_t(Class_t::* method)(Args...), const ArgVec& defaults) {
	return std::make_unique<Wrapper<Class_t, Method_t, Args...>>(obj, method, defaults);
}