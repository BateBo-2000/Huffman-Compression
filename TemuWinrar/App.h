#pragma once

#include "Execution/Invoker.h"
#include "Execution/Controller.h"
#include "UI/ConsoleInterface.h"
#include "Archive/ArchiveMaster.h"

class App {
public:
    App();
    void run();

private:
    void initCmd();

    Invoker invoker;
    ConsoleInterface ui;
    Controller controller;
    ArchiveMaster archive;
};
