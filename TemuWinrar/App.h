#pragma once

#include "Execution/Invoker.h"
#include "Execution/Controller.h"
#include "UI/ConsoleInterface.h"
#include "Archive/Archive.h"

class App {
public:
    App();
    void run();

private:
    void init();
    void initCmd();

    Invoker invoker;
    ConsoleInterface ui;
    Archive archive;
    Controller controller;
};
