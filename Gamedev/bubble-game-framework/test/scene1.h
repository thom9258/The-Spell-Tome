#define BU_IMPLEMENTATION
#include "../bubble_game_framework.h"

typedef struct {
    int counter;
}scene1_data;

void scene1_init(void)
{
    scene1_data* data = (scene1_data*)BU_scene_data_get();
    if (data == NULL)
        BU_FATAL("Could not access scene data!");

    data->counter = 0;
    printf("initialized counter to %d!\n", data->counter);
}

void scene1_update(void)
{
    BU_DEBUG("Running update of top scene.");
    scene1_data* data = (scene1_data*)BU_scene_data_get();
    if (data == NULL)
        BU_FATAL("Could not access scene data!");

    data->counter++;
    printf("updated counter (%d)\n", data->counter);
    if (data->counter >= 10)
        BU_scene_pop();
}

void scene1_destroy(void)
{
    BU_DEBUG("Destroying contents of scene");
}

BU_scene scene1 = BU_SCENE_NEW(scene1_init,
                               scene1_update,
                               scene1_destroy,
                               scene1_data);

