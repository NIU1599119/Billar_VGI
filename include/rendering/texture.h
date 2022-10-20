#pragma once

#include <glad/glad.h>

#include <string>

class Texture {
/*
    This class is temporal, it may be changed to enable model imports
*/
public:
    Texture() {};
    Texture(std::string src) : m_source(src) {};
    ~Texture() {};

    bool create();
    void activate();


private:
    unsigned int m_glTexture;

    std::string m_source;
};
