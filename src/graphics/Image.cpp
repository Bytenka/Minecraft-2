#include "pch.h"
#include "Image.h"

// Requirered to use stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb_image.h"

#include "../utils/Log.h"
#include "../utils/Exceptions.h"

namespace tk
{
Image::Image(const std::string &imgPath)
    : m_imgPath(imgPath)
{
    //stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(m_imgPath.c_str(), &m_width, &m_height, &m_nbChannels, 0);

    if (data == NULL)
    {
        throw RuntimeException("Unable to load file \"" + imgPath + "\"!\n\tReason: " + stbi_failure_reason());
    }

    // Discard previous data if existing
    if (m_data != NULL)
    {
        LOG_INFO("Reloading file \"{}\"", m_imgPath);
        stbi_image_free(m_data);
    }

    m_data = data;
    LOG_TRACE("Loaded image {}", imgPath);
}

Image::~Image()
{
    stbi_image_free(m_data);
}

//public:

//private::
} // namespace tk
