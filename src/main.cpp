#include "pch.h"
#include "application/Application.h"

#include "application/Window.h"
#include "graphics/Image.h"

int main(int argc, char *argv[])
{
    tk::Application &appli = tk::Application::getInstance();
    tk::WindowUID mainWindow = appli.createWindow("Yaaaaa", 1280, 720);
    appli.setMainWindow(mainWindow);

    tk::Window *w = appli.getInternalWindow(mainWindow);
    w->setIcon("res/icon.png");

    appli.runLoop();

    return 0;
}