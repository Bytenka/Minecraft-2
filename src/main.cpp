#include "pch.h"
#include "application/Application.h"

#include "application/Window.h"

int main(int argc, char *argv[])
{
    tk::Application &appli = tk::Application::getInstance();
    tk::WindowUID w = appli.createWindow("Oui");
    tk::WindowUID w2 = appli.createWindow("Oui2");
    tk::WindowUID w3 = appli.createWindow("Oui3");
    tk::WindowUID w4 = appli.createWindow("Oui4");
    appli.setMainWindow(w);
    appli.destroyWindow(w2);
    appli.destroyWindow(0);
    appli.destroyWindow(w);
    return 0;
}