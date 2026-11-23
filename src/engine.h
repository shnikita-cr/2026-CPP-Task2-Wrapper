#pragma once
#include <any>
#include <map>
#include <string>
#include "wrapper.h"

class Engine {
private:
	std::map<std::string, BigWrapper*> CommandList;

public:
	void register_command(BigWrapper* wrapper, const std::string& CommandName) { CommandList[CommandName] = wrapper; }

	std::string execute(const std::string& CommandName) { return CommandList.at(CommandName)->Execute(); }

	std::string execute(const std::string& CommandName, const std::map<std::string, std::any>& NewArgList) { return CommandList.at(CommandName)->Execute(NewArgList); }
};