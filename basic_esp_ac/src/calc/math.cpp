#include <iostream>
#include "math.h"



bool Math::WorldToScreen(Vector3 pos, Vector2& screen, std::array<float, 16> viewMatrix, int screenWidth, int screenHeight)
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

float DistanceTo(const Vector3& entPos, Vector3 locPlayerPos) {
    return sqrtf(
        (locPlayerPos.x - entPos.x) * (locPlayerPos.x - entPos.x) +
        (locPlayerPos.y - entPos.y) * (locPlayerPos.y - entPos.y) +
        (locPlayerPos.z - entPos.z) * (locPlayerPos.z - entPos.z));
}


bool Math::getRectPos(Vector3 locPlayerPos, Vector3 entPos, Vector2 screen, Vector2& start, Vector2& end)
{
    // Distance vom Spieler zum Gegner (Kamera zum Zielpunkt)
    float distance = DistanceTo(entPos, locPlayerPos); // musst du definieren
    if (distance <= 0.0f) distance = 0.01f; // vermeidet Division durch 0

    // Größe abhängig von Entfernung
    float scale = 100.0f / distance; // passt diesen Wert an für bessere Skalierung

    float height = scale * 50.0f;    // Basisgröße mal scale
    float width = height / 2.4f;

    //screen.x,y = (0 | 0) ist links oben ecke

    start.x = screen.x - (width / 2);
    start.y = screen.y;

    end.x = start.x + width;
    end.y = screen.y + height;

    return true;
}
