#include "Log.h"

namespace tk
{
std::shared_ptr<spdlog::logger> Log::s_mainLogger;

void Log::init() noexcept
{
    try
    {
        s_mainLogger = spdlog::stdout_color_mt("console");

#ifdef DEBUG
        s_mainLogger->set_level(spdlog::level::trace);
#else
        s_mainLogger->set_level(spdlog::level::info);
#endif
        s_mainLogger->set_pattern("[%T:%e] <%^%l%$> %v");
        s_mainLogger->debug("Initialized logger");
        LOG_DEBUG("Test");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
} // namespace tk