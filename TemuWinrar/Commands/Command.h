#pragma once
#include <string>
#include <vector>

class Command {
public:
    explicit Command(const std::string& name) : callName(name) {}
    virtual ~Command() = default;

    virtual const std::string& name() const noexcept { return callName; }

    virtual void execute(const std::vector<std::string>& args) = 0;
    virtual std::string description() const = 0;

private:
    std::string callName;
};
