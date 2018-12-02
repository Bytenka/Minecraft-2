#pragma once

#include "pch.h"
#include "Log.h"

#include <exception>

namespace tk
{
class RuntimeException : std::exception
{
  public:
    RuntimeException() : m_errMsg("") {}

    RuntimeException(const std::string &msg) : m_errMsg(msg)
    {
    }

    inline virtual const char *what() noexcept
    {
        return m_errMsg.c_str();
    }

  private:
    std::string m_errMsg;
};
} // namespace tk