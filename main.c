#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#include "scene_graph.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Scaling Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = loadTexture("sprite.png", renderer); 
    if (!texture) {
        SDL_Log("Unable to create texture");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    Node* root = createNode(renderer, 0.0f, 0.0f, 0.0f, 1.0f, texture);

	Node* childNode = 0;
	Node* lastNode = root;

	#define CHILD_COUNT 52

	Node* childs[CHILD_COUNT] = {0};

	int childId = 0;

	float scale = 1.0;

	float scaleIncrement = 1.0 / CHILD_COUNT;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, 50, 0.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}

	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, -50.f, 0.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}
	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, 0.f, 50.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}
	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, 0.f, -50.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= scaleIncrement;
	}

	for(int i = 0;i < CHILD_COUNT; ++i)
	{
		addAction(childs[i], rotationBehavior);
	}

    int running = 1;
    SDL_Event e;
    float angle = 0.0f;
	Uint32 next_time;
    while (running) {
		next_time = SDL_GetTicks() + TICK_INTERVAL;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

		recursiveUpdate(root);

        angle += 0.01f;

        SDL_RenderClear(renderer);


        drawNode(root, 400, 300, 0, 1);

        SDL_RenderPresent(renderer);

		SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }

    freeNode(root);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

void freeNode(Node* node) {
    for (int i = 0; i < node->numChildren; ++i) {
        freeNode(node->children[i]);
    }
    if (node->children != NULL) {
        free(node->children);
    }
	if (node->actions != NULL) {
        free(node->actions);
    }
    SDL_DestroyTexture(node->texture);
    free(node);
}
