#pragma once

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

class Math
{
public:
	bool WorldToScreen(Vector3 pos, Vector2& screen, float* viewMatrix, int screenWidth, int screenHeight);
};


