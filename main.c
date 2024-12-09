#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <math.h>

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
void rotationBehavior(Node* node);
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

	#define CHILD_COUNT 160

	Node* childs[CHILD_COUNT] = {0};

	int childId = 0;

	float scale = 1.;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, 50, 0.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= 1.0 / (CHILD_COUNT / 4);
	}

	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, -50.f, 0.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= 1.0 / (CHILD_COUNT / 4);
	}
	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, 0.f, 50.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= 1.0 / (CHILD_COUNT / 4);
	}
	scale = 1.;
	lastNode = root;

	for(int childCount = 0; childCount < CHILD_COUNT / 4;++childCount)
	{
		childNode = createNode(renderer, 0.f, -50.0f, 0.0f, scale, texture);
		addChild(lastNode, childNode);
		lastNode = childNode;
		childs[childId++] = childNode;
		scale -= 1.0 / (CHILD_COUNT / 4);
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

void rotationBehavior(Node* node)
{
	node->angle += 0.01f;
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