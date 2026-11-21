#ifndef ENGINE_H
#define ENGINE_H

#include "wrapper.h"
#include <string>
#include <map>
#include <stdexcept>

class Engine
{
private:
	std::map<std::string, Wrapper*> commands;

public:
	void register_command(Wrapper* wrapper, const std::string& alias) {
		commands[alias] = wrapper;
	}

	std::string execute(const std::string& alias, const Arguments& arguments = {}) {
		auto it = commands.find(alias);

		if (it == commands.end()) {
			throw std::runtime_error("Komanda ne naidena");
		}

		return it->second->execute(arguments);
	}
};


#endif