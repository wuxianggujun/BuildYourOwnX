#pragma once

namespace Globals
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	const float SPRITE_SCALE = 2.0f;
}

struct Vector2
{
	int x, y;

	Vector2() : x(0), y(0)
	{
	}

	Vector2(int x, int y): x(x), y(y)
	{
	}

	Vector2 zero()
	{
		return {0, 0};
	}
};
