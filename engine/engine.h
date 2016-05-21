/*
    TODO:
    Rename "game" to "platform"
    Rename "memoryPool" to "memoryStack"
    Write an actual "memoryPool" that can deallocate
*/

#define byte unsigned char
#define bool byte
#define true 1
#define false 0
#define new(type) (type*) calloc(1, sizeof(type))
#define null 0
#define KB(amount) amount * 1024
#define MB(amount) KB(amount) * 1024

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "memoryPool.h"
#include "utils.h"
#include "input.h"
#include "render.h"
#include "assets.h"
#include "game.h"
#include "collision.h"

#include "memoryPool.c"
#include "utils.c"
#include "input.c"
#include "render.c"
#include "assets.c"
#include "game.c"