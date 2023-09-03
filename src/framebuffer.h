#pragma once
#include "color.h"

const int WINDOW_WIDTH = 200;
const int WINDOW_HEIGHT = 200;
const int FRAMEBUFFER_WIDTH = 200;
const int FRAMEBUFFER_HEIGHT = 200;
const int FRAMEBUFFER_SIZE = FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT;

Color framebuffer[FRAMEBUFFER_SIZE];

Color clearColor = {0, 0, 0};
Color currentColor = {255, 255, 255};


void clear() {
    for (int i = 0; i < FRAMEBUFFER_SIZE; i++) {
        framebuffer[i] = clearColor;
    }
}