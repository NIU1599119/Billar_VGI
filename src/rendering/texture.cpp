#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "rendering/texture.h"


bool Texture::create()
{
    // textures
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(m_source.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        return false;
    }
    stbi_image_free(data);
    m_glTexture = texture;
    return true;
}

void Texture::activate(int id)
{
    // id is between 0 and 31 (can only have 32 textures assigned at the same time in the same shader)
    ASSERT(id < 32 && id >= 0);
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, m_glTexture);
}
