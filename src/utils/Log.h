#pragma once
#include "pch.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tk
{
class Log
{
public:
  static void init() noexcept;
  inline static std::shared_ptr<spdlog::logger> &getLogger() { return s_mainLogger; }

private:
  static std::shared_ptr<spdlog::logger> s_mainLogger;
  Log() {}
  ~Log() {}
};
} // namespace tk

#define LOG_TRACE(...) tk::Log::getLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) tk::Log::getLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...) tk::Log::getLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) tk::Log::getLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) tk::Log::getLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) tk::Log::getLogger()->critical(__VA_ARGS__)
