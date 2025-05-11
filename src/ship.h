#ifndef __SHIP_H__
#define __SHIP_H__

#include "scenegraph.h"
#include "type_helpers.h"

typedef struct ship_t
{
	node_t node;
	float velocity_s;
	SDL_Texture* texture_hit;
	SDL_Texture* texture_normal;
	SDL_Rect collision_rect;
} ship_t;

void ship_update(node_t* node, float delta_time)
{
	ship_t* ship = (ship_t*)node;
	const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

	vec2_t direction = {0};

	if(keyboard_state[SDL_SCANCODE_SPACE])
	{
		node->texture = ship->texture_hit;
	} else 
	{
		node->texture = ship->texture_normal;
	}

	if(keyboard_state[SDL_SCANCODE_LEFT])
	{
		direction.x = -1.;
	}

	if(keyboard_state[SDL_SCANCODE_RIGHT])
	{
		direction.x = 1.;
	}

	if(keyboard_state[SDL_SCANCODE_UP])
	{
		direction.y = -1.;
	}

	if(keyboard_state[SDL_SCANCODE_DOWN])
	{
		direction.y = 1.;
	}

	node->position = vec2_add(node->position, vec2_scale(vec2_normalize(direction), ship->velocity_s * delta_time));
}

ship_t* ship_create(SDL_Texture* texture, SDL_Texture* texture_hit)
{
	ship_t* ship = MALLOC_TYPE(ship_t);
	ship->velocity_s = 200.;
	node_add_behavior(ship, ship_update);

	ship->node.scale.x = ship->node.scale.y = 1.;
	ship->node.texture = texture;
	ship->texture_normal = texture;
	ship->texture_hit = texture_hit;
	ship->node.angle = 0;
	ship->collision_rect = (SDL_Rect){ .x = -5, .y = -5, .w = 10, .h = 10 };
	return ship;
}

#endif