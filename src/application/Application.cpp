#include "Application.h"

#include "../utils/Log.h"

namespace tk
{
Application::Application()
{
    Log::init();
    LOG_INFO("Application started");
}

Application::~Application()
{
}
} // namespace tk
