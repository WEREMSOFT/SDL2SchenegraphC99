#include "game.h"
#include <stdio.h>
#include <stdbool.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

game_t* game_p;

bool main_loop()
{
	return game_p->update();
}

void main_loop_web()
{
	game_p->update();
}

int main(void)
{
	game_p = game_create();

	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(main_loop_web, 0, false);
	#else
		while(main_loop());
		game_destroy();
	#endif
	
	return 0;
}


