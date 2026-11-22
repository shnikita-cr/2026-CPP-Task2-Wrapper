#include "Wrapper.h"
#include <string>
#include <memory>
#include <map>
#include <iostream>

class Engine {
public:
	Engine() = default;
	void register_command(IWrapper* _wrapperPtr, const std::string& _commandName) {
		command_map.emplace(_commandName, _wrapperPtr);
	}
	std::string execute(const std::string& _commandName, std::initializer_list<std::pair<const std::string, std::string>>&& _inArgs) {
		auto command = command_map.find(_commandName);


		if (command == command_map.end()) {
			std::cerr << "command not found" << std::endl;
		}
		else {
			if (command->second) {
				return command->second->call(std::move(_inArgs));
			}
			else
			{
				throw std::runtime_error("wrapper doesn't exist");
			}
		}
		return "";
	}
private:
	std::map<std::string, IWrapper*> command_map = {};
};