#pragma once

#include "pch.h"

namespace tk
{
class Image
{
  public:
    Image(const std::string &imgPath);
    ~Image();

    inline unsigned getNbChannels() const noexcept { return m_nbChannels; }
    inline unsigned getWidth() const noexcept { return m_width; }
    inline unsigned getHeight() const noexcept { return m_height; }
    inline unsigned char *getData() const noexcept { return m_data; }
    inline std::string getName() const noexcept { return m_imgPath.substr(0, m_imgPath.find(".") - 1); }

  private:
    int m_width, m_height, m_nbChannels;
    std::string m_imgPath;
    unsigned char *m_data = NULL;

  private:
    Image(const Image &) = delete;
    void operator=(const Image &) = delete;
};
} // namespace tk
