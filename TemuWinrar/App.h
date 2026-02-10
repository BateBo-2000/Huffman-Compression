#pragma once

class Invoker;
class Controller;
class UserInterface;
class Archive;

class App {
public:
    App();
    void run();

private:
    void init();
    void initCmd();
    

    Invoker* invoker;
    UserInterface* ui;
    Controller* controller;
    Archive* archive;
};
