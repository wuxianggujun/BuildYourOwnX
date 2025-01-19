#include <SDL2/SDL.h>
#include "Game.hpp"
#include "Graphics.hpp"
#include "Input.hpp"

namespace
{
	constexpr int FPS = 50;
	constexpr int MAX_FRAME_TIME = 5 * 1000 / FPS;
}

Game::Game()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	gameLoop();
}

Game::~Game()
{
}

void Game::gameLoop()
{
	Graphics graphics;
	Input input;
	SDL_Event event;

	_player = AnimatedSprite(graphics, "resource/sprites/MyChar.png", 0, 0, 16, 16, 100, 100,100);
	_player.setupAnimation();
	_player.playAnimation("RunLeft");
	
	int LAST_UPDATE_TIME = SDL_GetTicks();

	while (true)
	{
		input.beginNewFrame();

		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.repeat == 0)
				{
					input.keyDownEvent(event);
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				input.keyUpEvent(event);
			}
			else if (event.type == SDL_QUIT)
			{
				return;
			}
		}

		if (input.wasKeyPressed(SDL_SCANCODE_ESCAPE) == true)
		{
			return;
		}
		const int CURRENT_TIME_MS = SDL_GetTicks();
		int ELAPSED_TIME_MS = CURRENT_TIME_MS - LAST_UPDATE_TIME;
		update(std::min(ELAPSED_TIME_MS, MAX_FRAME_TIME));
		LAST_UPDATE_TIME = CURRENT_TIME_MS;
		draw(graphics);
	}
}

void Game::draw(Graphics& graphics)
{
	graphics.clear();
	_player.draw(graphics, 100, 100);
	graphics.flip();
}

void Game::update(float elapsedTime)
{
	_player.update(elapsedTime);
}
