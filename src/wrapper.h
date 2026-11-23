#pragma once
#include <string>
#include <map>
#include <any>

class BigWrapper {
public:
	BigWrapper() = default;
	virtual ~BigWrapper() = default;
	virtual std::string Execute(const std::map<std::string, std::any>& NewArgList) = 0;
	virtual std::string Execute() = 0;
};


template <typename SubjectType, typename RetFuncType, typename ...Args>
class Wrapper : public BigWrapper{
private:
	SubjectType* Subject;
	RetFuncType(SubjectType::*SubjectFunc)(Args...);
	const std::map<std::string, std::any> DefaultArgList;

	template <typename Arg, std::size_t I>
	Arg GetArg(const std::map<std::string, std::any>& NewArgList) {
		std::string st = "arg" + std::to_string(I + 1);
		return std::any_cast<Arg>(NewArgList.count(st) ? NewArgList.at(st) : DefaultArgList.at(st));
	}

	template <std::size_t ...Is>
	RetFuncType Execute2(std::index_sequence<Is...>, const std::map<std::string, std::any>& NewArgList) { return (Subject->*SubjectFunc)(GetArg<Args, Is>(NewArgList)...); }

	template <std::size_t ...Is>
	RetFuncType Execute2(std::index_sequence<Is...>) { return (Subject->*SubjectFunc)(std::any_cast<Args>(DefaultArgList.at("arg" + std::to_string(Is + 1)))...); }
	
	std::string toString(const std::string& str) { return str; }
	template <typename type>
	std::string toString(const type& str) { return std::to_string(str); }

public:
	Wrapper(SubjectType* Subj, RetFuncType(SubjectType::* SubjFunc)(Args...), std::map<std::string, std::any> List) : Subject(Subj), SubjectFunc(SubjFunc), DefaultArgList(std::move(List)) {}

	std::string Execute(const std::map<std::string, std::any>& NewArgList) override { return toString(Execute2(std::index_sequence_for<Args...>(), NewArgList));  }

	std::string Execute() override { return toString(Execute2(std::index_sequence_for<Args...>())); }

};
