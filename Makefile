SRC_F := $(shell find src -name *.c) $(shell find libs -name *.c)
SRC_CPP := $(shell find libs/soloud/src -name *.cpp)
OBJ_FOR_CLEAN_F := $(shell find ./src -name *.o)
SRC_O := $(patsubst %.c,%.o,$(SRC_F))
SRC_CPP_O := $(patsubst %.cpp,%.o,$(SRC_CPP))

LIBS := -lm -lSDL2 -lSDL2_gfx -lSDL2_image

FLAGS_DEBUG := -g -O0 -w
FLAGS__DEBUG := -O3
FLAGS := -Wall -Wextra -Ilibs/include -L./static_libs -Ilibs/cimgui

# Vars for emscripten build
EMSC_CFLAGS := -O2 -s -Wall -D_DEFAULT_SOURCE -DWITH_MINIAUDIO=1 -s MIN_WEBGL_VERSION=2 -Wno-missing-braces -s OFFSCREEN_FRAMEBUFFER=1 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -s USE_GLFW=3 -sFULL_ES3 -s TOTAL_MEMORY=67108864 --preload-file assets -v -D OS_WEB
EMSC_CC := emcc

TARGET := bin/main.bin
.PONY: clean web statistics

all: clean $(SRC_O) $(SRC_CPP_O) copy_assets
	gcc $(FLAGS_DEBUG) $(FLAGS) $(SRC_O) $(SRC_CPP_O) -o $(TARGET) $(LIBS)

run_main: all
	$(TARGET)

%.o: %.c
	gcc -c $(FLAGS_DEBUG) $(FLAGS) $^ -o $@

%.o: %.cpp
	gcc -c $(FLAGS_RELEASE) $(FLAGS) -lstdc++ -ldl -lasound -DWITH_ALSA=1 $^ -o $@

main.bin: clean src/mainNew.c
	gcc -g -O0 src/mainNew.c -lm -o main.bin

main_clean.bin:


web:
	rm -rf docs
	mkdir docs
	emcc -O0 -g -sUSE_SDL=2 -s USE_LIBPNG=1 --use-preload-plugins -sUSE_SDL_GFX=2 -sUSE_SDL_IMAGE=2 -sSTACK_SIZE=1024000 -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" -sEXPORTED_FUNCTIONS=_malloc,_free,_main -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -DWITH_MINIAUDIO=1 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 --preload-file assets -s MIN_WEBGL_VERSION=2 -gsource-map  $(SRC_F) $(SRC_CPP) -o docs/index.html
#	emcc -O0 -g -Ilibs/include -Ilibs/soloud/include -Ilibs/soloud/include -Ilibs/cimgui  -sSTACK_SIZE=1024000 -sEXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" -sEXPORTED_FUNCTIONS=_malloc,_free,_main -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -DWITH_MINIAUDIO=1 -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=10  -DCIMGUI_USE_GLFW=1 -DCIMGUI_USE_OPENGL3=1 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 --preload-file assets -s MIN_WEBGL_VERSION=2 -gsource-map --source-map-base http://127.0.0.1:5500/docs/  $(SRC_F) $(SRC_CPP) ./static_libs/cimgui.a -o docs/index.html
	rm -rf $(OBJ_FOR_CLEAN_F)
	(cd docs && zip -r index.zip .)

run_web: web
	emrun --port 8080 ./docs/.

clean:
	rm -rf $(OBJ_FOR_CLEAN_F)
	rm -rf $(TARGET)
	rm -rf bin/assets
	rm -rf docs
	rm -rf html/**/*.*
	rm -rf ./main.bin

deep_clean: clean
	rm -rf $(SRC_CPP_O)
	

copy_assets:
	cp -r assets bin

statistics_old:
	echo >> metrics.txt
	git log -1 --format="%H - %s" >> metrics.txt
	pmccabe -vt src/main.c >> metrics.txt

statistics:
	echo >> metrics.txt
	git log -1 --format="%H - %s" >> metrics.txt
	complexity --histogram --score --thresh=3 `ls src/*.c` >> metrics.txt

create_report:
	sed -e '/Total/,/-/!d' metrics.txt 
