#pragma once

class Invoker;
class UserInterface;

class Controller {
public:
	Controller(Invoker& inv, UserInterface& ui);	//Not taking ownership
	void start();
private:
	Invoker& invoker;
	UserInterface& ui;
};