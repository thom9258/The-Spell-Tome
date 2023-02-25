#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define BU_IMPLEMENTATION
#include "../bubble_game_framework.h"

#include "scene1.h"

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
    const int memsize = 1024*4;
    void* memory = malloc(memsize);
    if (memory == NULL)
        return -1;

    BUBBLE_config conf = {0};
    conf.main_memory = memory;
    conf.main_memory_size = memsize;

    BUBBLE_init(conf);
    BU_scene_push(scene1);
    BUBBLE_run();
    BUBBLE_destroy();

	return 0;
}

