#include <iostream>
#include "math.h"



bool Math::WorldToScreen(Vector3 pos, Vector2& screen, float viewMatrix[16], int screenWidth, int screenHeight)
{
    float x = viewMatrix[0] * pos.x + viewMatrix[4] * pos.y + viewMatrix[8] * pos.z + viewMatrix[12];
    float y = viewMatrix[1] * pos.x + viewMatrix[5] * pos.y + viewMatrix[9] * pos.z + viewMatrix[13];
    float w = viewMatrix[3] * pos.x + viewMatrix[7] * pos.y + viewMatrix[11] * pos.z + viewMatrix[15];


    if (w < 0.01f)
        return false;

    float invW = 1.0f / w;
    screen.x = (screenWidth / 2.0f) + (x * invW * screenWidth / 2.0f);
    screen.y = (screenHeight / 2.0f) - (y * invW * screenHeight / 2.0f);

    return true;
}
