#pragma once
#include <SDL_rect.h>
#include <SDL_render.h>
#include <string>

class Graphics;

class Sprite
{
public:
	Sprite();
	explicit Sprite(Graphics& graphics, const std::string& filePath, int sourceX, int sourceY,
	                int width, int height, float posX, float posY);

	virtual ~Sprite();
	virtual void update();
	void draw(Graphics& graphics, int x, int y);
protected:
	SDL_Rect _sourceRect;
	SDL_Texture* _spriteSheet;
private:
	float _x, _y;
};
