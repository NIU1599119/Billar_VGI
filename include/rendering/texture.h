#pragma once

#include <glad/glad.h>

#include <string>
#include "debug.h"

class Texture {
/*
    This class is temporal, it may be changed to enable model imports
*/
public:
    Texture() {};
    Texture(std::string src) : m_source(src) {};
    ~Texture() {};

    bool create();
    void activate(int id = 0); // default id is 0


private:
    unsigned int m_glTexture;

    std::string m_source;
};
