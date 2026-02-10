#include "Invoker.h"
#include "../Commands/Command.h"
#include "../Utils/splitWords.inl"
#include <cassert>
#include <stdexcept>

Invoker::Invoker(): shouldExit(false)
{}

Invoker::~Invoker()
{
	for (size_t i = 0; i < commands.size(); i++)
		delete commands[i];

	commands.clear();
}

void Invoker::registerCommand(Command* cmd)
{
	assert(cmd!= nullptr && "Invoker::registerCommand: null cmd");

	for (size_t i = 0; i < commands.size(); i++)
		assert(commands[i]->name() != cmd->name() && "Invoker: RegisterCommand: Command already exists");

	commands.push_back(cmd);
}


//Throws exceptions.
void Invoker::execute(const std::string& commandLine)
{
	std::vector<std::string> args = splitWords(commandLine);
	Command* cmd = findCommand(args[0]);
	if (!cmd) throw std::invalid_argument("Command " + args[0] + " not found.");

	cmd->execute(args);
}

Command* Invoker::findCommand(const std::string& name)
{
	for (size_t i = 0; i < commands.size(); i++)
	{
		if (commands[i]->name() == name) return commands[i];
	}


	return nullptr;
}
