#include "Graphics.hpp"
#include "Globls.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Graphics::Graphics()
{
	SDL_CreateWindowAndRenderer(Globals::SCREEN_WIDTH, Globals::SCREEN_HEIGHT, 0, &this->_window, &this->_renderer);
	SDL_SetWindowTitle(this->_window, "Covestory");

}

Graphics::~Graphics()
{
	SDL_DestroyWindow(this->_window);
}

SDL_Surface* Graphics::loadImage(const std::string& filePath)
{
	if (_spriteSheets.count(filePath) == 0)
	{
		_spriteSheets[filePath] = IMG_Load(filePath.c_str());

	}
	return _spriteSheets[filePath];
}

void Graphics::blitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle)
{
	SDL_RenderCopy(_renderer, texture, sourceRectangle, destinationRectangle);

}

void Graphics::flip()
{
	SDL_RenderPresent(_renderer);

}

void Graphics::clear()
{
	SDL_RenderClear(_renderer);
}

SDL_Renderer* Graphics::getRenderer() const
{
	return _renderer;
}
