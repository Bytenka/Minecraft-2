#pragma once

namespace tk
{
class Application
{
  public:
    inline static Application &getInstance()
    {
        static Application instance;
        return instance;
    }

  private:
    Application();
    ~Application();

  public:
    Application(const Application &) = delete;
    void operator=(const Application &) = delete;
};
} // namespace tk

// Singleton pattern from https://stackoverflow.com/questions/1008019/c-singleton-design-pattern/1008289#1008289