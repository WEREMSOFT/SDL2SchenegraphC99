all:
	gcc -g -O0 -o main.bin main.c -lm -lSDL2 -lSDL2_gfx -lSDL2_image

run: all
	./main.bin
