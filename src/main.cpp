#include "pch.h"
#include "application/Application.h"

#include "application/Window.h"

int main(int argc, char *argv[])
{
    tk::Application &appli = tk::Application::getInstance();
    tk::WindowUID mainWindow = appli.createWindow("Yaaaaa", 1280, 720);
    tk::WindowUID mainWindow2 = appli.createWindow("Yaaaaa", 1280, 720);
    tk::WindowUID mainWindow3 = appli.createWindow("Yaaaaa", 1280, 720);
    tk::WindowUID mainWindow4 = appli.createWindow("Yaaaaa", 1280, 720);
    appli.setMainWindow(mainWindow);

    tk::Window *w = appli.getInternalWindow(mainWindow);

    appli.runLoop();

    return 0;
}