#pragma once
#include "App.h"

// commands
#include "Commands/ExitCmd.h"
#include "Commands/HelpCmd.h"
#include "Commands/CheckCmd.h"
#include "Commands/InfoCmd.h"
#include "Commands/UnzipCmd.h"
#include "Commands/UpdateCmd.h"
#include "Commands/ZipCmd.h"

App::App() : controller(invoker, ui)
{
	init();
}

void App::run()
{
    controller.start();
}

void App::init()
{
	initCmd();
}

void App::initCmd()
{
    // service
    invoker.registerCommand(new ExitCmd(invoker));
    invoker.registerCommand(new HelpCmd(invoker, ui));

    // app
    invoker.registerCommand(new CheckCmd(ui, archive));
    invoker.registerCommand(new InfoCmd(ui, archive));
    invoker.registerCommand(new UnzipCmd(ui, archive));
    invoker.registerCommand(new UpdateCmd(ui, archive));
    invoker.registerCommand(new ZipCmd(ui, archive));
}
