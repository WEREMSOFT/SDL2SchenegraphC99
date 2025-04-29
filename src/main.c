#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include "scene_graph.h"

void rotateXAction(Node* node)
{
	static float phase = 0;
	node->x = sin(phase+=-.05) * 100 + 50;
}

void rotateYAction(Node* node)
{
	static float phase = 0;
	node->y = cos(phase+=-.05) * 100 + 50;
}

int running = 1;
float angle = 0.0f;
Node* root;
Node* childNode = 0;


#define CHILD_COUNT 52

Node* childs[CHILD_COUNT] = {0};


Game game;

void main_loop()
{
	static Uint32 next_time;
	next_time = SDL_GetTicks() + TICK_INTERVAL;

	running = handleQuitEvents();

	recursiveUpdate(root);

	angle += 0.01f;

	SDL_RenderClear(game.renderer);

	drawNode(root, 400, 300, 0, 1);

	SDL_RenderPresent(game.renderer);

	SDL_Delay(time_left());
	next_time += TICK_INTERVAL;
}

int main(int argc, char* argv[]) {
   
	game = gameCreate("This is an unnamed window", 800, 600);

    SDL_Texture* texture = loadTexture("assets/sprite.png", game.renderer); 
    if (!texture) {
        SDL_Log("Unable to create texture");
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

	root = createNode(game.renderer, 0.0f, 0.0f, 0.0f, 1.0f, texture);

	Node* lastNode = root;

	int childId = 0;

	float scale = 1.0;

	float scaleIncrement = 1.0 / CHILD_COUNT;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(game.renderer, 50, 0.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}

	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(game.renderer, -50.f, 0.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}
	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(game.renderer, 0.f, 50.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}
	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(game.renderer, 0.f, -50.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}

	for(int i = 0;i < CHILD_COUNT; ++i)
	{
		addAction(childs[i], rotationBehavior);
	}


#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, false);
#else
	while (running) {
		main_loop();
	}
    freeNode(root);
    SDL_DestroyTexture(texture);
	gameDestroy(game);
#endif

    return 0;
}


