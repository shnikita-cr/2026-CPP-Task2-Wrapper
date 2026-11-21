
#ifndef WRAPPER_H
#define WRAPPER_H

#include <string>
#include <map>
#include <functional>

using Arguments = std::map<std::string, std::string>;

class Wrapper {
private:
	std::function<std::string(const Arguments&)> function;
	Arguments default_arguments;

public:
	template <typename T>
	Wrapper(T* object, std::string(T::* method)(const Arguments&), const Arguments& defaults) : default_arguments(defaults) {
		function = [object, method](const Arguments& args) {
				return (object->*method)(args);
		};
	}

	std::string execute(const Arguments& arguments = {}) {
		Arguments current_arguments = default_arguments;

		for (const auto& argument : arguments) {
			current_arguments[argument.first] = argument.second;
		}

		return function(current_arguments);
	}
};

#endif