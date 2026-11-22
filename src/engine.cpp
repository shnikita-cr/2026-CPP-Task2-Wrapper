#include "engine.h"

void Engine::register_command(BaseWrapper* wrap, const std::string& command_name) {
	commands[command_name] = wrap;
}
