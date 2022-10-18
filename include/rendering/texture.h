#pragma once

#include <glad/glad.h>

#include <string>

class Texture {
public:
    Texture() {};
    Texture(std::string src) : m_source(src) {};
    ~Texture() {};

    bool createTexture();
    void activateTexture();


private:
    unsigned int m_glTexture;

    std::string m_source;
};
