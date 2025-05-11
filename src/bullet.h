#ifndef __BULLET_H__
#define __BULLET_H__

#include "scenegraph.h"
#include "type_helpers.h"

typedef struct bullet_t 
{
	node_t node;
	vec2_t velocity;
} bullet_t;

uint64_t bullet_count = 0;

DEFINE_ARRAY_TYPE_DYNAMIC(arr_available_bullets_t, bullet_t *, 2);

arr_available_bullets_t* bullet_pool;

void bullet_update(node_t* node, float delta_time)
{
	bullet_t *bullet = (bullet_t*)node;
	
	bullet->node.position = vec2_add(node->position, vec2_scale(bullet->velocity, delta_time));

	if(node->position.y > 590 || node->position.y < 10 || node->position.x > 790 || node->position.x < 10 )
	{
		bullet_destroy(node);
	}
}

bullet_t* bullet_create(SDL_Texture* texture, vec2_t center, float angle, float velocity_s)
{
	bullet_t *bullet = NULL;

	if(arr_available_bullets_t_pop(bullet_pool, &bullet)==-1)
	{
		bullet_count++;
		bullet = calloc(sizeof(bullet_t), 1);
		node_add_behavior(bullet, bullet_update);	
	}

	bullet->node.scale.x = bullet->node.scale.y = 1.;
	bullet->node.texture = texture;
	bullet->node.angle = 0;
	
	float angle_sin = sin(angle);
	float angle_cos = cos(angle);

	vec2_t velocity = {.x = angle_sin * velocity_s, .y = angle_cos * velocity_s};

	bullet->velocity = velocity;


	
	return bullet;
}

void bullet_destroy(bullet_t* bullet)
{
	bullet->node.disabled = true;
	arr_available_bullets_t_push(bullet_pool, bullet);
}

#endif
