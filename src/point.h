#pragma once
#include "color.h"
#include "framebuffer.h"
#include "fragment.h"

void setCurrentColor(const Color color)
{
    currentColor = color;
}

void point(Fragment f)
{
    int index = f.y * FRAMEBUFFER_WIDTH + f.x;

    // Verificar si el índice está dentro de la pantalla
    if (index >= 0 && index < FRAMEBUFFER_SIZE)
    {
        // Colocar color del pixel (x,y)
        framebuffer[index] = currentColor;
    }
}