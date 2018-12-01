#pragma once
#include "pch.h"

namespace tk
{
class Log
{
  public:
    static void init();

  private:
    static std::shared_ptr<spdlog::logger> mainLogger;
};
} // namespace tk