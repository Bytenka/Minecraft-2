#pragma once

#include "Window.h"

#define WINDOW_NULL 0

namespace tk
{
typedef unsigned long long WindowUID;
class Application
{
public:
  inline static Application &getInstance()
  {
    static Application instance;
    return instance;
  }

  WindowUID createWindow(const std::string &title, unsigned width = 1280, unsigned height = 720);
  void destroyWindow(WindowUID uid);

  inline void setMainWindow(WindowUID uid) noexcept { m_mainWindowUID = uid; }

private:
  std::vector<std::pair<WindowUID, std::unique_ptr<Window>>> m_windows;
  WindowUID m_mainWindowUID = WINDOW_NULL;
  WindowUID m_windowUIDCounter = 1;

private:
  Application();
  ~Application();

public:
  Application(const Application &) = delete;
  void operator=(const Application &) = delete;
};
} // namespace tk

// Singleton pattern from https://stackoverflow.com/questions/1008019/c-singleton-design-pattern/1008289#1008289