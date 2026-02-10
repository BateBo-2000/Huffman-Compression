#pragma once
#include <string>
#include <vector>

class Command {
public:
	explicit Command(const std::string& name) : cmdName(name) {}
	virtual ~Command() = default;

	virtual bool execute(const std::vector<std::string>& args) = 0;
	virtual const std::string& name() const = 0;
	virtual const std::string& description() const = 0;

private:
	std::string cmdName;
};