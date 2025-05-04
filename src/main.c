#include "scenegraph.h"
#include <stdio.h>
#include <stdbool.h>

bool main_loop(SDL_Renderer* renderer, node_t* root)
{
	static Uint32 old_time;
	Uint32 new_time;
	
	if(old_time == 0) old_time = SDL_GetTicks();
	new_time = SDL_GetTicks();

	float delta_time = (new_time - old_time) / 1000.;

	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN:
				if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					return false;

				if(e.key.keysym.scancode == SDL_SCANCODE_R)
					SDL_RenderClear(renderer);
					
				break;
			
		}
	}

	root->angle += 1.0 * delta_time;

	root->childs[0]->angle += 1.0 * delta_time;

	// SDL_RenderClear(renderer);

	node_draw(root, NULL, global_coordinates_create(), 0, renderer);

	SDL_RenderPresent(renderer);

	old_time = new_time;
	return true;
}

int main(void)
{
	static Uint32 old_time;

	if(SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Error initializing SDL: %s\n", SDL_GetError());
		exit(-1);
	}

	SDL_Window* window = SDL_CreateWindow("NodeTest", 0, 0, 800, 600, SDL_WINDOW_SHOWN);

	if(window == NULL)
	{
		printf("Error creating window: %s\n", SDL_GetError());
		exit(-1);
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	node_t* root = node_create("assets/sprite.png", renderer);
	
	root->position.x = 400;
	root->position.y = 300;
	root->scale.x = root->scale.y = .8;
	root->angle = 1.;

	node_t* child = node_create("assets/sprite.png", renderer);

	child->position.x = 41;
	child->position.y = 0;

	child->scale.x = child->scale.y = .9;

	node_add_child(child, child);

	node_add_child(root, child);

	while(main_loop(renderer, root));

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	return 0;
}

