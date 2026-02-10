#pragma once
#include <vector>
#include <string>

class Command;

class Invoker {
public:
	Invoker();
	~Invoker();

	//not planning on using these
	Invoker(const Invoker& other) = delete;
	Invoker& operator=(const Invoker& other) = delete;

	void registerCommand(Command* cmd); //takes ownership of the command
	void execute(const std::string& commandLine);
	bool exit() { return shouldExit; }

private:
	Command* findCommand(const std::string& name);

	std::vector<Command*> commands;
	bool shouldExit;
};