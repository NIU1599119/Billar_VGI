#pragma once

#include "window.h"
#include "audio.h"

int initMenu(int& opcio, Window& window, float& x, float& y);

int initResolution(float& x, float& y, Window& window);

int optionsMenu(Window& window, float& Width, float& Height, irrklang::ISound* music, int& musicNum, float& volume);