#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// How many times is allowed to process myself as mychild?
#define MAX_SELF_CHILD_COUNT 50 


// Vectors
typedef struct vec2_t {
	float x;
	float y;
} vec2_t;

vec2_t vec2_add(vec2_t a, vec2_t b)
{
	return (vec2_t) {
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}

vec2_t vec2_scale(vec2_t a, float scale)
{
	return (vec2_t) {
		.x = a.x * scale,
		.y = a.y * scale
	};
}

// Coordinate System
typedef struct global_coordinates_t
{
	vec2_t scale;
	vec2_t position;
	double angle;
} global_coordinates_t;

global_coordinates_t global_coordinates_create()
{
	return (global_coordinates_t){
		.scale = {.x = 1., .y = 1.},
		.position = {0},
		.angle = 0.
	};
}

global_coordinates_t global_coordinates_propagate(global_coordinates_t a, global_coordinates_t b)
{
	return (global_coordinates_t)
	{
		.angle = a.angle + b.angle,
		.position = vec2_add(a.position, b.position),
		.scale = (vec2_t){.x = a.scale.x * b.scale.x, .y = a.scale.y * b.scale.y}
	};
}

// Scenegraph
typedef struct node_t;
typedef void (*behavior_function_t)(struct node_t*, float deltaTime);

typedef struct node_t {
	SDL_Texture* texture;
	vec2_t position;
	vec2_t scale;
	double angle;
	struct node_t** childs;
	int child_count;
	behavior_function_t* behaviors;
	int behavior_count;
	bool disabled;
} node_t;

void node_update(struct node_t* node, node_t* parent, Sint32 loop_count, float delta_time)
{
	if(node->disabled) return;
	if(loop_count >= MAX_SELF_CHILD_COUNT) return;

	for(int i = 0; i < node->behavior_count; i++)
	{
		node->behaviors[i](node, delta_time);
	}

	for(int i = 0; i < node->child_count; i++)
	{
		node_update(node->childs[i], node, node==parent?loop_count+1:0, delta_time);
	}
}

void node_draw(struct node_t* node, struct node_t* parent, global_coordinates_t global_coordinates, Sint32 loop_count, SDL_Renderer* renderer)
{
	if(node->disabled) return;
	if(loop_count >= MAX_SELF_CHILD_COUNT) return;

	// obtener seno y coseno
	float sin_a = sin(global_coordinates.angle);
	float cos_a = cos(global_coordinates.angle);

	// obtener la posicion rotada usando la forula rot_x = x' * sin(a) + y' * cos(a); rot_y = x' * sin(a) - y' * cos(a)
	float rot_x = node->position.x * cos_a - node->position.y * sin_a;
	float rot_y = node->position.x * sin_a + node->position.y * cos_a;

	// obtener la x e y final sumando rot_x y rot_y a global_x e global_y
	float final_x = global_coordinates.position.x + rot_x * global_coordinates.scale.x;
	float final_y = global_coordinates.position.y + rot_y * global_coordinates.scale.y;

	// obtener la escala final multiplicando las escalas.
	float scale_x = global_coordinates.scale.x * node->scale.x;
	float scale_y = global_coordinates.scale.y * node->scale.y;

	if(node->texture != NULL)
	{
		// obtener las medidas de la textura.
		int text_size_x, text_size_y;
	
		SDL_QueryTexture(node->texture, NULL, NULL, &text_size_x, &text_size_y);
		
		// armar el rectangulo con las medidas de la textura multiplicada por la escala final y la posicion final restada la mitad del tamanio de la textura multiplicado por la escala final.
		SDL_Rect texture_position = 
		{
			.w = text_size_x * scale_x,
			.h = text_size_y * scale_y,
			.x = final_x - text_size_x * .5 * scale_x,
			.y = final_y - text_size_x * .5 * scale_y
		};
		float final_angle = global_coordinates.angle + node->angle;
		// render
	
		SDL_RenderCopyEx(renderer, node->texture, NULL, &texture_position, final_angle * (180.0f / M_PI), NULL, SDL_FLIP_NONE);
	}

	// procesar los hijos
	global_coordinates.angle += node->angle;
	global_coordinates.position.x = final_x;
	global_coordinates.position.y = final_y;
	global_coordinates.scale.x = scale_x;
	global_coordinates.scale.y = scale_y;

	for(int i = 0; i < node->child_count; i++)
	{
		node_draw(node->childs[i], node, global_coordinates, node == parent?loop_count+1:0, renderer);
	}
}

node_t* node_create_from_texture(SDL_Texture* texture)
{
	node_t* node = calloc(sizeof(node_t), 1);
	node->scale.x = node->scale.y = 1.;
	node->texture = texture;
	node->angle = 0;
	return node;
}

node_t* node_create(char* texture_file_path, SDL_Renderer* renderer)
{
	SDL_Texture* texture = IMG_LoadTexture(renderer, texture_file_path);
	return node_create_from_texture(texture);
}

void node_add_child(struct node_t* node, struct node_t* child)
{
	node->childs = realloc(node->childs, sizeof(node_t) * ++node->child_count);
	node->childs[node->child_count-1] = child;
}

void node_add_behavior(struct node_t* node, behavior_function_t* behavior)
{
	node->behaviors = realloc(node->behaviors, sizeof(node_t) * ++node->behavior_count);
	node->behaviors[node->behavior_count-1] = behavior;
}

#endif
