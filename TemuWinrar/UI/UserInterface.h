#pragma once
#include <string>

class UserInterface {
public:
	virtual ~UserInterface() = default;
	virtual void ask(const std::string& question) = 0;
	virtual std::string readLine() = 0;
	virtual void inform(const std::string& info) = 0;
	virtual void warn(const std::string& warning) = 0;
	virtual void error(const std::string& error) = 0;
};

//member functions aren't const to enable logging.