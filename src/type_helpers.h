#ifndef __TYPE_HELPERS_H__
#define __TYPE_HELPERS_H__

#define DEFINE_ARRAY_TYPE(NAME, TYPE, SIZE) \
typedef struct NAME { \
	TYPE values[SIZE]; \
	int count; \
	int capacity; \
} NAME;

#define DEFINE_ARRAY_TYPE_DYNAMIC(NAME, TYPE, SIZE) \
typedef struct NAME { \
	TYPE* values; \
	int count; \
	int capacity; \
} NAME;\
DEFINE_ARRAY_DYNAMIC_CREATE(TYPE, 2, NAME);\
DEFINE_ARRAY_PUSH(TYPE, NAME);\
DEFINE_ARRAY_POP(TYPE, NAME);\

#define MALLOC_TYPE(TYPE) (TYPE*)calloc(sizeof(TYPE), 1)

#define DEFINE_ARRAY_DYNAMIC_CREATE(TYPE, SIZE, ARRAY_TYPE) \
ARRAY_TYPE* ARRAY_TYPE##_create() \
{\
	ARRAY_TYPE* return_value = MALLOC_TYPE(ARRAY_TYPE);\
	return_value->values = (TYPE*)calloc(sizeof(TYPE), SIZE);\
	return_value->capacity = SIZE;\	
	return return_value;\	
}

#define DEFINE_ARRAY_PUSH(TYPE, TYPE_ARRAY) \
void TYPE_ARRAY##_push(TYPE_ARRAY* array, TYPE value) \
{\
	if(!(array->count < array->capacity))\
	{\
		array->values = (TYPE *)realloc(array->values, sizeof(TYPE) * array->capacity * 2);\
		array->capacity*=2;\
	}\
	array->values[array->count++] = value;\
}

#define DEFINE_ARRAY_POP(TYPE, TYPE_ARRAY) \
int TYPE_ARRAY##_pop(TYPE_ARRAY* array, TYPE* value) \
{\
	if(array->count > 0)\
	{\
		*value = array->values[--array->count];\
		return 0;\
	}\
	return -1;\
}

#endif