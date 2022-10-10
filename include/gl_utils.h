#pragma once

#include "stdio.h"
// Aixó es del codi original, serveix per imprimir errors que podrien passar amb opengl

#ifdef DEBUG
#define GL(x)                                                                              \
    x;                                                                                     \
    {                                                                                      \
        int glError = glGetError();                                                        \
        if (glError != GL_NO_ERROR)                                                        \
        {                                                                                  \
            fprintf(stderr, "OpenGL Error: %d near '%s:%d'\n", glError, __FILE__, __LINE__); \
            exit(glError);                                                                 \
        }                                                                                  \
    }
#else
#define GL(x) x
#endif
