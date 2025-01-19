#pragma once

#include <map>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <string>

struct SDL_Window;

class Graphics
{
public:
	Graphics();
	~Graphics();
	SDL_Surface* loadImage(const std::string& filePath);

	void blitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle);

	void flip();

	void clear();

	[[nodiscard]] SDL_Renderer* getRenderer() const;

private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;

	std::map<std::string, SDL_Surface*> _spriteSheets;
};
