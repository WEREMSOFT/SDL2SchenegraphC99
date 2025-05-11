#ifndef __BULLET_POOL_H__
#define __BULLET_POOL_H__

#include "type_helpers.h"
#include "bullet.h"

#define MAX_BULLETS 300

DEFINE_ARRAY_TYPE(ar_bullet_pool_t, bullet_t*, MAX_BULLETS);
DEFINE_ARRAY_TYPE(ar_free_bullet_t, int, MAX_BULLETS);

ar_bullet_pool_t bullet_pool;

DEFINE_ARRAY_TYPE_DYNAMIC(ar_free_bullet_t, bullet_t, 1);
ar_free_bullet_t* free_bullets;
DEFINE_ARRAY_PUSH(bullet_t, ar_free_bullet_t);
DEFINE_ARRAY_POP(bullet_t, ar_free_bullet_t);
DEFINE_ARRAY_DYNAMIC_CREATE(bullet_t, 2, ar_free_bullet_t);


#endif