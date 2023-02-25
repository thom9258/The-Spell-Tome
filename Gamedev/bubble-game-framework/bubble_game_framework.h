/*
ORIGINAL AUTHOR:
Thomas Alexgaard Jensen (https://github.com/thom9258)

LICENSE:
Copyright (c) <Thomas Alexgaard Jensen>
This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages arising
from the use of this software. Permission is granted to anyone to use this
software for any purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.

For more information, please refer to
<https://choosealicense.com/licenses/zlib/>

CHANGELOG:
[0.1] Implemented First iteration of:
        - Messaging
        - Scene management
        - Memory Management
        - Main bubble context

[0.2] Implemented a config struct as input to BUBBLE_init().
      Started impl on double frame buffer.
      Fixed top scene pointer to be a function for safety reasons.
[0.3] Replaced scene stack impl with an templated array.


TODO:
- Replace build-in messaging for my external implementation.
- Implement a BU_double_frame_allocator implementation using 2
  BU_stack_allocator's.
- Implement a BU_timer system.
- Use BU_timer to implement:
  + BU_deltatime
  + Settable framerate for update loop
- Figure out if we want a draw function to run in a seperate thread as
  part of the bubble implementation or if its something we want to manage
  seperately in a graphics library.
- (maybe) Implement a c Templated stack

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUBBLE_GAME_FRAMEWORK_H
#define BUBBLE_GAME_FRAMEWORK_H

/*Standard libs*/
#include <stdio.h>


/*Custom libs*/
#define TH_ALLOCATOR_IMPLEMENTATION
#include "vendor/th_allocator.h"


/*******************************************************************************
 * Common defs
 *******************************************************************************/

#define BU_API static inline
#define BU_BACKEND static

#define BU_SCENE_STACK_SIZE 32
#define BU_MSG_STACK_SIZE 128

#define BU_IGNORE(i)
  

/*******************************************************************************
 * Error and Message Manager
 *******************************************************************************/

enum BU_STATUS {
	BU_STATUS_DEBUG = 0,
	BU_STATUS_INFO,
	BU_STATUS_ERROR,
	BU_STATUS_FATAL,

	BU_STATUS_COUNT,
};

/*Simple Converter from enum BU_STATUS to its string reprecentation*/
const char*
_BU_STATUSSTR[BU_STATUS_COUNT] = {
    (char*)"DEBUG",
    (char*)"INFO",
    (char*)"ERROR",
    (char*)"FATAL",
};

#define BU_ERR_OK "BU_OK"
#define BU_ERR_UNKNOWN "BU_UNKNOWN"
#define BU_ERR_INVALID_INPUT "BU_INVALID_INPUT"
#define BU_ERR_INVALID_ALLOC "BU_INVALID_ALLOC"

typedef struct {
	enum BU_STATUS status;
	char* msg;

	/*Automatic stuff no need to set manually*/
	int _LINE_;
	char* _FILE_;
	int msg_index;
}BU_msg;

/*Simple Message pushing utils*/
#define BU_MSG(errstatus, err) \
	(BU_msg) {errstatus, (char*)err, __LINE__, (char*)__FILE__, -1}
#define BU_DEBUG(err) BU_msg_push(BU_MSG(BU_STATUS_DEBUG, err))
#define BU_INFO(err)  BU_msg_push(BU_MSG(BU_STATUS_INFO,  err))
#define BU_ERROR(err) BU_msg_push(BU_MSG(BU_STATUS_ERROR, err))
#define BU_FATAL(err) BU_msg_push(BU_MSG(BU_STATUS_FATAL, err))

typedef struct {
	BU_msg msgs[BU_MSG_STACK_SIZE];
	int top;
	void (*callback)(void);
	unsigned int total_msg_count;
}BU_msgManager;

BU_API
void BU_msg_push(BU_msg _msg);

BU_API
BU_msg BU_msg_pop();

BU_API
void BU_msg_clear();

BU_API
void BU_msg_callback_stdout();


/*******************************************************************************
 * Memory and Allocators
 *******************************************************************************/

typedef struct {
    th_stackallocator primary;
    th_stackallocator secondary;
}BU_double_frameallocator;


BU_IGNORE(

 BU_API
void BU_double_frameallocator_init(uint32_t _n);

BU_BACKEND
void _BU_double_frameallocator_destroy();

BU_BACKEND
void _BU_double_frameallocator_swap();

BU_API
void BU_quickalloc(uint32_t _n);

)

BU_API
void* BU_malloc(uint32_t _n);

BU_IGNORE(
        
BU_API
void* BU_realloc(void* _p, uint32_t _n);

    )

BU_API
void BU_free(void* _ptr);


/*******************************************************************************
 * Scene Manager
 *******************************************************************************/


typedef void(*BU_scene_fn)(void);

typedef struct {
    BU_scene_fn init;
    BU_scene_fn update;
    BU_scene_fn destroy;
    void* data;
    uint32_t data_size;

}BU_scene;

typedef struct {
    BU_scene scene; 
    void* data;
    BU_double_frameallocator df_allocator;
}BU_scene_content;

#define TSARRAY_NAME_OVERRIDE
#define t_type BU_scene
#define arr_t_name _BU_scene_manager
#include "vendor/tsarray.h"

/*Create scenes statically so it can be done in scene files at compile time*/
#define BU_SCENE_NEW(init, update, destroy, data) \
    (BU_scene) {init, update, destroy, NULL, sizeof(data)}

BU_API
void* BU_scene_data_get();

BU_API
void BU_scene_data_set(void* _scene_data_ptr);

BU_API
int BU_scene_push(BU_scene _sc);

BU_API
void BU_scene_pop();

BU_API
int BU_scene_count();


/*******************************************************************************
 * Bubble Context
 *******************************************************************************/


struct _BUBBLE_CONTEXT {
    BU_msgManager msg_manager;
    _BU_scene_manager scene_manager;
    th_allocator main_allocator;
};

/*Global bubble context*/
struct _BUBBLE_CONTEXT BUBBLE;

typedef struct {
    void* main_memory;
    uint32_t main_memory_size;
    uint32_t target_updates_per_second;
    int scene_stack_size;
}BUBBLE_config;


BU_API
void BUBBLE_init(BUBBLE_config _conf);
    
BU_API
void BUBBLE_destroy();

BU_API
void BUBBLE_run();


/*******************************************************************************
 * Bubble Game Framework Implementation
 * 
 *******************************************************************************/
#define BU_IMPLEMENTATION
#ifdef BU_IMPLEMENTATION

BU_API
void
BU_msg_push(BU_msg _msg)
{
    /*make stack a circle buffer*/
	if (BUBBLE.msg_manager.top >= BU_MSG_STACK_SIZE)
        BUBBLE.msg_manager.top = 0;

    /*Push msg onto stack*/
	_msg.msg_index = BUBBLE.msg_manager.total_msg_count++;
	BUBBLE.msg_manager.msgs[BUBBLE.msg_manager.top++] = _msg;

    /*Do a callback on the error stack.
	 *Determine what to do with pushed error*/
	if (BUBBLE.msg_manager.callback != NULL)
		BUBBLE.msg_manager.callback();
}

BU_API
BU_msg
BU_msg_pop()
{
	if (BUBBLE.msg_manager.top > 0)
		return BUBBLE.msg_manager.msgs[--BUBBLE.msg_manager.top];
	return BU_MSG(BU_STATUS_ERROR, "No msg found in stack.");
}

BU_API
void
BU_msg_clear()
{
	while (BUBBLE.msg_manager.top > 0)
		BU_msg_pop();
}

BU_API
void BU_msg_callback_stdout()
{
    BU_msg top = BU_msg_pop();
    printf("[%s(%d)]\t(%s L%d) %s\n",
           _BU_STATUSSTR[top.status],
           top.msg_index,
           top._FILE_,
           top._LINE_,
           top.msg);
}


BU_BACKEND
BU_scene* _BU_scene_top_ptr()
{
    BU_scene* top;
    if (_BU_scene_manager_len(&BUBBLE.scene_manager) < 0){
        BU_FATAL("Tried to access top scene but none existed.");
        return NULL;
    }
    top =_BU_scene_manager_peek(&BUBBLE.scene_manager, -1);
    if (top == NULL)
        BU_FATAL("top element of scene manager is NULL");
    return top;
}

BU_API
void* BU_scene_data_get()
{
    if (_BU_scene_manager_len(&BUBBLE.scene_manager) < 0)
        return NULL;
    return _BU_scene_top_ptr()->data;
}

BU_API
void BU_scene_data_set(void* _scene_data_ptr)
{
    _BU_scene_top_ptr()->data = _scene_data_ptr;
}

BU_API
int
BU_scene_push(BU_scene _sc)
{
    if (_sc.update == NULL)
        BU_ERROR(BU_ERR_INVALID_INPUT);

    _BU_scene_manager_push(&BUBBLE.scene_manager, _sc);

    _BU_scene_top_ptr()->data = BU_malloc(_sc.data_size);
    if (_BU_scene_top_ptr()->data == NULL)
        BU_FATAL("Could not allocate Scene data.");

    if (_BU_scene_top_ptr()->init != NULL)
        _BU_scene_top_ptr()->init();

    BU_INFO("Pushed a scene to stack!");
    return _BU_scene_manager_len(&BUBBLE.scene_manager);
}

BU_API
void
BU_scene_pop()
{
    BU_scene pop;
    if (_BU_scene_manager_len(&BUBBLE.scene_manager) < 1) {
        BU_ERROR("Tried to pop a scene, but no scene existed!");
        return;
    }

    pop = _BU_scene_manager_pop(&BUBBLE.scene_manager);
    if (pop.destroy != NULL)
        pop.destroy();

    BU_free(pop.data);

    BU_INFO("Popped a scene from stack!");
}

BU_API
int BU_scene_count()
{
    return _BU_scene_manager_len(&BUBBLE.scene_manager);
}

BU_API
void*
BU_malloc(uint32_t _n)
{
    if (_n < 1) {
        BU_ERROR("Trying to allocate invalid size!");
        return NULL;
    }
    return th_allocator_malloc(&BUBBLE.main_allocator, _n);
}

BU_IGNORE(

BU_API
void*
BU_realloc(void* _p, uint32_t _n)
{
    if (_n < 1) {
        BU_ERROR("Trying to allocate invalid size!");
        return NULL;
    }
    return th_allocator_realloc(&BUBBLE.main_allocator, _p, _n);
}
)

BU_API
void
BU_free(void* _ptr)
{
    if (_ptr == NULL) {
        BU_ERROR("Trying to free invalid ptr!");
        return;
    }
    th_allocator_free(&BUBBLE.main_allocator, _ptr);
}

BU_API
void
BUBBLE_init(BUBBLE_config _conf)
{
    th_status a_ok;
    /*Initialize message stack*/
    BUBBLE.msg_manager.callback = BU_msg_callback_stdout;

    /*Initialize main allocator*/
    a_ok = th_allocator_init(&BUBBLE.main_allocator,
                             (uint8_t*)_conf.main_memory,
                             _conf.main_memory_size);
    if (a_ok != TH_OK)
        BU_FATAL("Could not create the main allocator!");
    BU_INFO("Correctly initialized main allocator.");

    if (_conf.scene_stack_size < 1)
        _conf.scene_stack_size = 8;
    BUBBLE.scene_manager = _BU_scene_manager_initn(_conf.scene_stack_size);
    if (_BU_scene_manager_capacity(&BUBBLE.scene_manager) != _conf.scene_stack_size)
        BU_FATAL("Could not create a scene manager!");
    BU_INFO("Created Scene manager.");

    BU_INFO("BUBBLE Context Initialized.");
}

BU_API
void BUBBLE_destroy()
{
    _BU_scene_manager_destroy(&BUBBLE.scene_manager);
    BU_INFO("BUBBLE Context Destroyed.");
}


BU_API
void
BUBBLE_run()
{
    if (_BU_scene_manager_len(&BUBBLE.scene_manager) < 1)
        BU_FATAL("Running without any scenes.");

    while (_BU_scene_manager_len(&BUBBLE.scene_manager) > 0) {
        if (_BU_scene_top_ptr()->update == NULL)
            BU_ERROR("Top Scene has no update()");
        _BU_scene_top_ptr()->update();
    }
}


#endif /*BU_IMPLEMENTATION*/
#endif /*BUBBLE_GAME_FRAMEWORK_H*/

#ifdef __cplusplus
}
#endif
