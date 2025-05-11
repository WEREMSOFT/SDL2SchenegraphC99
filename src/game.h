#ifndef __GAME_H__
#define __GAME_H__

#include "scenegraph.h"
#include <stdbool.h>
#include "type_helpers.h"
#include "bullet.h"
#include "ship.h"

typedef bool (*update_function_t)();

DEFINE_ARRAY_TYPE_DYNAMIC(arr_bullets_t, node_t*, 30);
DEFINE_ARRAY_TYPE(arr_textures_t, SDL_Texture*, 30);

typedef struct game_t
{
	arr_textures_t textures;
	arr_bullets_t* bullets;
	node_t* root;
	ship_t* ship;
	update_function_t update;
	SDL_Renderer* renderer;
	SDL_Window* window;
} game_t;

static game_t game = {0};

void game_add_texture(const char* texture_file_path)
{
	game.textures.values[game.textures.count++] = IMG_LoadTexture(game.renderer, texture_file_path);
}

void update_root(node_t* node, float delta_time)
{
	node->angle += 1.0 * delta_time;
	node->childs[0]->angle += 1.1 * delta_time;
}

void show_fps()
{
	static Uint32 fps;
	static Uint32 last_time;
	Uint32 new_time = SDL_GetTicks();

	if(last_time == 0)
	{
		last_time = SDL_GetTicks();
	}

	fps++;

	if(new_time - last_time >= 1000)
	{
		char* window_title[500] = {0};
		sprintf(window_title, "particle count %d - fps %d", bullet_count, fps);
		SDL_SetWindowTitle(game.window, window_title);
		last_time = new_time;
		fps = 0;
	}


}

void shot_bullets(float delta_time)
{
	static float shoot_delay = 0.01;

	static float elapsed_time;

	elapsed_time += delta_time;

	if(elapsed_time < shoot_delay)
	{
		return;
	}

	elapsed_time = 0;
	static float initial_angle = 0.;
	static float emisor_angle = 0.;
	static float emisor_radious = 100.;
	static float initial_angle_phase;
	static float speed_phase;
	speed_phase += 1.5 * delta_time;
	initial_angle_phase += 1.2 * delta_time;
	initial_angle += .1 * sin(initial_angle_phase);
	emisor_angle += 2.1 * delta_time;
	
	vec2_t emissor_position = {.x = 400. + sin(emisor_angle) * emisor_radious, .y  = 300. + cos(emisor_angle) * emisor_radious};

	#define NUM_BULLETS 500
	for(int i = 0; i < NUM_BULLETS; i++)
	{
		bullet_t* bullet = bullet_create(game.textures.values[1], emissor_position, i * 360. / NUM_BULLETS + initial_angle, 200 + 100. * sin(speed_phase));
		bullet->node.position = emissor_position;
		if(bullet->node.disabled)
		{
			bullet->node.disabled = false;
		} else {
			arr_bullets_t_push(game.bullets, bullet);
			node_add_child(game.root, bullet);
		}
	}
}

bool check_if_ship_was_hit()
{
	SDL_Rect ship_rect = game.ship->collision_rect;
	
	ship_rect.x += game.ship->node.position.x;
	ship_rect.y += game.ship->node.position.y;

	for(int i = 0; i < game.bullets->count; i++)
	{
		SDL_Rect bullet_rect = {0};
		SDL_QueryTexture(game.bullets->values[i]->texture, NULL, NULL, &bullet_rect.w, &bullet_rect.h);
		bullet_rect.x = game.bullets->values[i]->position.x - bullet_rect.w / 2;
		bullet_rect.y = game.bullets->values[i]->position.y - bullet_rect.h / 2;

		SDL_Rect intersection = {0};

		bool result = SDL_IntersectRect(&bullet_rect, &ship_rect, &intersection);
		if(result)
		{
			return true;
		}
	}
	return false;
}

bool game_update()
{
	static Uint32 old_time;
	Uint32 new_time;
	static Uint32 fps = 0;

	if(old_time == 0) old_time = SDL_GetTicks();
	new_time = SDL_GetTicks();

	float delta_time = (new_time - old_time) / 1000.;

	SDL_Event e;

	show_fps();

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
					SDL_RenderClear(game.renderer);
					break;
		}
	}

	shot_bullets(delta_time);

	node_update(game.root, NULL, 0, delta_time);

	if(check_if_ship_was_hit())
	{
		game.ship->node.texture = game.ship->texture_hit;
	} else
	{
		game.ship->node.texture = game.ship->texture_normal;
	}

	SDL_RenderClear(game.renderer);

	node_draw(game.root, NULL, global_coordinates_create(), 0, game.renderer);

	SDL_RenderPresent(game.renderer);

	old_time = new_time;
	return true;
}

game_t* game_create()
{
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Error initializing SDL: %s\n", SDL_GetError());
		exit(-1);
	}

	game.window = SDL_CreateWindow("NodeTest", 0, 0, 800, 600, SDL_WINDOW_SHOWN);

	if(game.window == NULL)
	{
		printf("Error creating window: %s\n", SDL_GetError());
		exit(-1);
	}

	game.bullets = arr_bullets_t_create();

	game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);// | SDL_RENDERER_PRESENTVSYNC);

	game_add_texture("assets/sprite.png");
	game_add_texture("assets/bullet.png");
	game_add_texture("assets/ship.png");
	game_add_texture("assets/ship_hit.png");
	game.root = node_create_from_texture(game.textures.values[0]);

	ship_t* ship = ship_create(game.textures.values[2], game.textures.values[3]);

	game.ship = ship;

	ship->node.position.x = 400;
	ship->node.position.y = 500;

	node_add_child(game.root, ship);

	game.root->position.x = 0;
	game.root->position.y = 0;

	game.update = game_update;

	bullet_t* bullet = calloc(sizeof(bullet_t), 1);
	bullet->node.texture = game.textures.values[1];
	
	bullet->node.position.x = 400;
	bullet->node.position.y = 300;
	bullet->node.scale.x = bullet->node.scale.y = 1.;
	arr_bullets_t_push(game.bullets, bullet);
	node_add_child(game.root, bullet);

	bullet_pool = arr_available_bullets_t_create();


	return &game;
}

void game_destroy()
{
	SDL_DestroyRenderer(game.renderer);
	SDL_DestroyWindow(game.window);
}



#endif