#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <math.h>

struct Node;

typedef void (*ActionFunction)(struct Node*, float);

typedef struct Node {
    float x, y;           
    float angle;          
    float scale;          
    struct Node* parent;
    SDL_Renderer* renderer;
    struct Node** children;
    int numChildren;
    SDL_Texture* texture;
	ActionFunction* actions;
    int numActions;
} Node;

void recursiveUpdate(Node* node);
void rotationBehavior(Node* node, float);
void freeNode(Node* node);

Node* createNode(SDL_Renderer* renderer, float x, float y, float angle, float scale, SDL_Texture* texture) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->x = x;
    node->y = y;
    node->angle = angle;
    node->scale = scale;
    node->parent = NULL;
    node->renderer = renderer;
    node->children = NULL;
    node->numChildren = 0;
    node->numActions = 0;
    node->texture = texture;
	node->actions = NULL;
    return node;
}

#define TICK_INTERVAL    30

static Uint32 next_time;

Uint32 time_left(void)
{
    Uint32 now;

    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

void addChild(Node* parent, Node* child) {
    parent->numChildren++;
    parent->children = (Node**)realloc(parent->children, sizeof(Node*) * parent->numChildren);
    parent->children[parent->numChildren - 1] = child;
    child->parent = parent;
}

void addAction(Node* parent, ActionFunction action) {
    parent->numActions++;
    parent->actions = (ActionFunction*)realloc(parent->actions, sizeof(ActionFunction) * parent->numActions);
    parent->actions[parent->numActions - 1] = action;
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        SDL_Log("Error al cargar la imagen %s: %s", path, IMG_GetError());
        return NULL;
    }

    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (newTexture == NULL) {
        SDL_Log("Error al crear textura desde superficie %s: %s", path, SDL_GetError());
    }

    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

void drawNode(Node* node, float parentX, float parentY, float parentAngle, float parentScale) {
    float cosTheta = cos(parentAngle);
    float sinTheta = sin(parentAngle);

    float rotatedX = node->x * cosTheta - node->y * sinTheta;
    float rotatedY = node->x * sinTheta + node->y * cosTheta;

    float absX = parentX + rotatedX * parentScale;
    float absY = parentY + rotatedY * parentScale;

    float finalScale = parentScale * node->scale;

    float finalAngle = parentAngle + node->angle;

    SDL_Rect dstRect;
    int textureWidth, textureHeight;
    SDL_QueryTexture(node->texture, NULL, NULL, &textureWidth, &textureHeight);

    dstRect.x = absX - (textureWidth * finalScale / 2);
    dstRect.y = absY - (textureHeight * finalScale / 2);
    dstRect.w = textureWidth * finalScale;
    dstRect.h = textureHeight * finalScale;

    SDL_RenderCopyEx(node->renderer, node->texture, NULL, &dstRect, finalAngle * (180.0f / M_PI), NULL, SDL_FLIP_NONE);

    for (int i = 0; i < node->numChildren; ++i) {
        drawNode(node->children[i], absX, absY, finalAngle, finalScale);
    }
}

void rotationBehavior(Node* node, float angle)
{
	static float rotationIncrement = 0;
	static float sinIncrement = 0.01;
	sinIncrement += 0.0001;
	rotationIncrement = sin(sinIncrement) * 0.05;
	node->angle += rotationIncrement;
}

void scaleBehavior(Node* node)
{
	node->scale = 1.0 + cos(node->angle) * 0.1;
}

void recursiveUpdate(Node* node)
{
	for (int i = 0; i < node->numActions; ++i) {
		node->actions[i](node ,0.f);
	}

	for (int i = 0; i < node->numChildren; ++i) {
		recursiveUpdate(node->children[i]);
	}
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

typedef struct Game
{
	SDL_Window* window;
	SDL_Renderer* renderer;
} Game;

Game gameCreate(char *windowName, int windowWidth, int windowHeight)
{
	Game game = {0};

 	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(-1);
    }

	#ifndef __EMSCRIPTEN__
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        exit(-1);
    }
	#endif
    game.window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!game.window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        exit(-1);
    }

    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game.renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(game.window);
        IMG_Quit();
        SDL_Quit();
        exit(-1);
    }

	return game;
}

void gameDestroy(Game game)
{
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    IMG_Quit();
    SDL_Quit();
}

int handleQuitEvents()
{
    SDL_Event e;
	int running = 1;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			running = 0;
		} else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE) { // Salir si se presiona Esc
				running = 0;
			}
		}
	}

	return running;
}
#endif