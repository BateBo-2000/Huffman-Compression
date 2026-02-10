#include "Controller.h"
#include "Invoker.h"
#include "../UI/UserInterface.h"
#include <string>
#include <exception>


Controller::Controller(Invoker& inv, UserInterface& ui): invoker(inv), ui(ui)
{}

void Controller::start()
{
	std::string line;
	while (!invoker.exit()) {
		try {
			line = ui.readLine();
			if (line.empty()) continue;
			invoker.execute(line);
		}
		catch (const std::exception& e) {
			ui.error(e.what());
		}
	}
}
