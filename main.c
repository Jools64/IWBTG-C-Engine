#define PI 3.14159265359

#define NO_MUSIC

#include "engine/engine.h"
#include "defaultMap.h"
#include "menu.h"
#include "editor.h"

// TODO: Stop being so lazy and break up this into headers and c source files.

// HEADER

#define MAX_ENTITIES 2048
#define MAP_WIDTH 30
#define MAP_HEIGHT 17
#define GRID_SIZE 32

typedef enum
{
    EntityType_spike,
    EntityType_smallSpike,
    EntityType_block,
    EntityType_save,
    EntityType_warp,
    EntityType_particle,
    EntityType_playerBullet,
    EntityType_fruit,
    EntityType_movingPlatform
} EntityType;

typedef enum
{
    ControllerType_none,
    ControllerType_trap,
    ControllerType_inOut,
    ControllerType_chain
} ControllerType;

typedef struct ControllerTrap
{
    float direction;
    float speed;
    float activationDistance;
    float activationWidth;
    int activationFrame;
    unsigned char activated;
} ControllerTrap;

typedef struct ControllerInOut
{
    float speed;
    float distance;
    float direction;
    float interval;
    float timer;
    Vector2f basePosition;
} ControllerInOut;

typedef struct Controller Controller;
typedef struct Controller
{
    ControllerType type;
    union
    {
        ControllerTrap trap;
        ControllerInOut inOut;
    };
    unsigned char hasChains;
    //Entity* owner;
} Controller;

typedef struct
{
    Vector2f position, velocity, acceleration;
    Sprite sprite;
    EntityType type;
    bool active;
    float spin, spinFriction, friction;
    float animationTimer;
    float depth;
    Controller* controller;
    Controller controllerData;
} Entity;

typedef struct 
{
    Vector2f position, velocity;
    Sprite sprite;
    Rectanglef hitBox;
    float runSpeed, gravity,
          jumpSpeed, doubleJumpSpeed,
          jumpDampening, maxFallSpeed;
    bool doubleJumpAvailible, dead;
} Player;

typedef enum
{
    Animations_default,
    Animations_stand,
    Animations_run,
    Animations_jump,
    Animations_fall,
    Animations_slide
} Animations;

typedef enum
{
    GameState_menu,
    GameState_inGame,
    GameState_gameOver
} GameState;

typedef struct
{
    Vector2f playerPosition;
    Vector2i room;
    int deaths;
    double time;
} SaveState;

typedef struct
{
    Grid entities;
    Grid ground;
    Grid controllers;
    Entity* entityMap[MAP_WIDTH][MAP_HEIGHT];
} Level;

typedef struct Iwbtg
{
    Game game;
    Player player;
    
    //Vector2f checkPointPosition;
    //int checkPointRoom;
    SaveState saveState;
    
    Texture* blockTexture;
    Texture* backgroundTexture;
    Texture* cloudsTexture;
    Texture* gameOverTexture;
    Texture* titleTexture;
    
    Sprite gameOverSprite;
    
    Level level;
    Editor editor;
    
    Menu mainMenu, loadMenu;
    Menu* activeMenu;
    
    Entity entities[MAX_ENTITIES];
    Entity* entityDrawOrder[MAX_ENTITIES];
    int lastEntityPosition, entityDrawCount,
        activeSaveSlot, frameCount;
    Vector2i room;
    
    float time;
    
    GameState state;
    float gameOverTimer;
    
    Vector2f debugDrawPosition;
} Iwbtg;

// C FILES

#include "menu.c"
#include "editor.c"

// IMPLEMENTATION

char* getCurrentMapName(Iwbtg* iw, char* string, int stringLength)
{
    snprintf(string, stringLength, "assets/%dx%d.map", iw->room.x, iw->room.y);
    return string;
}

Entity* getFirstFreeEntity(Iwbtg* iw)
{
    int i;
    // Circular search starting at the last found empty position.
    for(i = iw->lastEntityPosition; i < MAX_ENTITIES; ++i)
        if(!iw->entities[i].active)
        {
            iw->lastEntityPosition = i;
            return &iw->entities[i];
        }
        
    for(i = 0; i < iw->lastEntityPosition; ++i)
        if(!iw->entities[i].active)
        {
            iw->lastEntityPosition = i;
            return &iw->entities[i];
        }
        
    return 0;
}

int entityCount(Iwbtg* iw, EntityType type)
{
    int count = 0;
    
    for(int i = 0; i < MAX_ENTITIES; ++i)
    {
        Entity* e = &iw->entities[i];
        if(e->active && e->type == type)
            ++count;
    }
    
    return count;
}

/*
// For debugging the per pixel collisions
bool checkRectangleIntersectSpriteDraw(Rectanglef* r, Vector2f* rp, Sprite* s, Vector2f* sp, Iwbtg* iw)
{
    // Get the sprite frame
    int tX = (s->frame * s->size.x) % s->texture->size.x,
        tY = ((s->frame * s->size.x) / s->texture->size.x) * s->size.y;
        
    int deltaX = -sp->x;
    int deltaY = -sp->y;
    
    double xxc, yxc, xyc, yyc;

    if(s->angle == 0) // For absolute precision when no rotation has taken place
    {
        xxc = 1;
        yxc = 0;
        xyc = 0;
        yyc = 1;
    }
    else
    {
        xxc = cos(s->angle);
        yxc = - sin(s->angle);
        xyc = cos(s->angle + (PI * 1.5));
        yyc = - sin(s->angle + (PI * 1.5));
    }
    
    // Iterate through every pixel in the rectangle
    for(int i = rp->x + r->x; i <= rp->x + r->x + r->w; ++i)
        for(int t = rp->y + r->y; t <= rp->y + r->y + r->h; ++t)
        {
            bool collision = false;
            
            Texture* texture = s->texture;
            double x = i + deltaX, 
                  y = t + deltaY, 
                  sx = tX, 
                  sy = tY;

            double ox = sx + s->origin.x;
            double oy = sy + s->origin.y;
            
            x += sx;
            y += sy;
            
            double ix = (x) - ox;
            double iy = (y) - oy;
            
            x = (((xxc * ix) + (xyc * iy))/s->scale.x) + ox;
            y = (((yxc * ix) + (yyc * iy))/s->scale.y) + oy;
            
            if(x >= 0 && x >= sx 
            && y >= 0 && y >= sy
            && x < s->texture->size.x && x < sx + s->size.x
            && y < s->texture->size.y && y < sy + s->size.y)
            {
                unsigned char* data = (unsigned char*) s->texture->surface->pixels;
                int index = (((int)x + ((int)y * s->texture->size.x))*4) + 3;
                collision = data[index] > 0;
                if(collision)
                    SDL_RenderDrawPoint(iw->game.renderer, i, t);
            }
            
            //if(collision)
                //return true;
        }
        
    return false;
}*/

Entity* createEntity(Iwbtg* iw, EntityType type, float x, float y)
{
    Entity* e = getFirstFreeEntity(iw);
    if(e == 0)
    {
        printf("Error: Out of space for entities!\n");
        return 0;
    }
    
    e->position.x = x;
    e->position.y = y;
    e->spin = 0;
    e->spinFriction = 0;
    e->friction = 0;
    e->velocity.x = e->velocity.y = e->acceleration.x = e->acceleration.y = 0;
    e->active = true;
    e->type = type;
    e->animationTimer = 0;
    e->depth = 0;
    e->controller = &e->controllerData;
    e->controller->hasChains = false;
    e->controller->type = ControllerType_none;
    
    // Important: All entities must have a sprite. (Or I need to implement behavior to handle this)
    switch(type)
    {
        case EntityType_block:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "blockTiles"), 16, 16);
            e->depth = -1;
			break;
            
        case EntityType_spike:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "spike"), 32, 32);
            e->sprite.origin.x = 16; //e->sprite.size.x / 2;
            e->sprite.origin.y = 16; //e->sprite.size.y / 2;
            break;
            
        case EntityType_save:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "save"), 32, 32);
            break;
            
        case EntityType_warp:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "warp"), 128, 128);
            e->depth = -1;
            break;
            
        case EntityType_playerBullet: {
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "playerBullet"), 4, 4);
            int frames[] = { 0, 1 };
            spriteAddAnimation(&e->sprite, Animations_default, &frames[0], 2, 12);
            spritePlayAnimation(&e->sprite, Animations_default);
            e->depth = -2;
        } break;
        
        case EntityType_fruit:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "fruit"), 32, 32);
            int frames[] = { 0, 1 };
            spriteAddAnimation(&e->sprite, Animations_default, &frames[0], 2, 6);
            spritePlayAnimation(&e->sprite, Animations_default);
            break;
            
        case EntityType_movingPlatform:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "movingPlatform"), 32, 16);
            e->depth = -1;
            break;
    }
    
    return e;
}

Entity* createParticle(Iwbtg* iw, Texture* texture, float x, float y, 
                       float hSpeed, float vSpeed, float life)
{
    Entity* e = createEntity(iw, EntityType_particle, x, y);
    
    e->velocity.x = hSpeed;
    e->velocity.y = vSpeed;
    e->animationTimer = life;
    e->spin = 1;
    e->friction = 2;
    e->spinFriction = 0.05;
    e->depth = -2;

    spriteInit(&e->sprite, texture, texture->size.x, texture->size.y);
    e->sprite.additiveBlend = true;
    
    return e;
}

void destroyEntity(Entity* e)
{
    e->active = false;
}

void destroyAllEntities(Iwbtg* iw)
{
    for(int i = 0; i < MAX_ENTITIES; ++i)
        iw->entities[i].active = false;
}

void addChainLink(Level* l, unsigned char chainLinks[MAP_WIDTH][MAP_HEIGHT], int x, int y, Controller** joinedController)
{
    if(x > 0 && y > 0 && x < MAP_WIDTH && y < MAP_HEIGHT 
       && chainLinks[x][y] == 0)
    {
        int c = l->controllers.data[x + (y * l->entities.width)] - 1;

        if(c == 63) // chain link
        {
            chainLinks[x][y] = true;
            addChainLink(l, chainLinks, x-1, y, joinedController);
            addChainLink(l, chainLinks, x+1, y, joinedController);
            addChainLink(l, chainLinks, x, y-1, joinedController);
            addChainLink(l, chainLinks, x, y+1, joinedController);
        }
        else if(c != -1 && l->entityMap[x][y]) // controller is something other than chain link
            *joinedController = l->entityMap[x][y]->controller;
    }
}

void resolveChain(Entity* e, Iwbtg* iw)
{
    unsigned char chainLinks[MAP_WIDTH][MAP_HEIGHT];
    Controller* joinedController = 0;
    memset(chainLinks, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));
    addChainLink(&iw->level, chainLinks, e->position.x / 32, e->position.y / 32, &joinedController);
    
    if(joinedController != 0)
    {
        joinedController->hasChains = true;
        for(int i = 0; i < MAP_WIDTH; ++i)
            for(int t = 0; t < MAP_HEIGHT; ++t)
            {
                if(chainLinks[i][t])
                {
                    Entity* e = iw->level.entityMap[i][t];
                    if(e)
                        e->controller = joinedController;
                }
            }
    }
}

void loadMap(Iwbtg* iw, char* file)
{
    FILE* f;
    if(f = fopen(file, "r"))
    {
        fread(iw->level.entities.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
        fread(iw->level.controllers.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
        memset(iw->level.ground.data, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->level.entityMap, 0, sizeof(Entity*) * MAP_WIDTH * MAP_HEIGHT); 
        fclose(f);
    }
    else
    {
        memcpy(iw->level.entities.data, defaultMap, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->level.ground.data, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->level.ground.data, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->level.entityMap, 0, sizeof(Entity*) * MAP_WIDTH * MAP_HEIGHT); 
        iw->editor.enabled = true;
    }
    
    destroyAllEntities(iw);
    
    for(int t = 0; t < iw->level.entities.height; ++t)
        for(int i = 0; i < iw->level.entities.width; ++i)
        {
            int index = i + (t * iw->level.entities.width);
            int typeIndex = iw->level.entities.data[index] - 1;
            
            int x = 32 * i,
                y = 32 * t;
            
            Entity* e = 0;
            
            if(typeIndex == 0)
            {
                iw->level.ground.data[i + (t * iw->level.ground.width)] = 1;
                e = createEntity(iw, EntityType_block, x, y);
            }
            else if(typeIndex >= 2 && typeIndex <= 5)
            {
                e = createEntity(iw, EntityType_spike, x, y);
                e->sprite.frame = typeIndex - 2;
            }
            else if(typeIndex == 6)
                e = createEntity(iw, EntityType_fruit, x, y);
            else if(typeIndex == 10)
                e = createEntity(iw, EntityType_save, x, y);
            else if(typeIndex == 11)
                e = createEntity(iw, EntityType_warp, x-48, y-48);
            else if(typeIndex == 1)
            {
                iw->player.position.x = x;
                iw->player.position.y = y;
            }
            else if(typeIndex == 7)
                e = createEntity(iw, EntityType_movingPlatform, x, y);
            
            iw->level.entityMap[i][t] = e;
            
            // Add a controller if one has been applied to the entity
            if(e)
            {
                typeIndex = iw->level.controllers.data[index] - 1;
                
                e->controller = &e->controllerData;
                
                if(typeIndex >= 0 && typeIndex <= 3) // Fast trap
                {
                    e->controller->type = ControllerType_trap;
                    e->controller->trap.direction = PI * 0.5f * typeIndex;
                    e->controller->trap.activationDistance = 400;
                    e->controller->trap.activationWidth = 32;
                    e->controller->trap.speed = 32;
                    e->controller->trap.activated = 0;
                }
                else if(typeIndex >= 4 && typeIndex <= 7) // Medium trap
                {
                    e->controller->type = ControllerType_trap;
                    e->controller->trap.direction = PI * 0.5f * (typeIndex - 4);
                    e->controller->trap.activationDistance = 400;
                    e->controller->trap.activationWidth = 32;
                    e->controller->trap.speed = 16;
                    e->controller->trap.activated = 0;
                }
                else if(typeIndex >= 8 && typeIndex <= 11) // Medium trap
                {
                    e->controller->type = ControllerType_trap;
                    e->controller->trap.direction = PI * 0.5f * (typeIndex - 8);
                    e->controller->trap.activationDistance = 400;
                    e->controller->trap.activationWidth = 32;
                    e->controller->trap.speed = 4;
                    e->controller->trap.activated = 0;
                }
                else if(typeIndex >= 12 && typeIndex <= 13) // In out
                {
                    e->controller->type = ControllerType_inOut;
                    e->controller->inOut.direction = PI * 0.5;
                    e->controller->inOut.speed = 1;
                    if(typeIndex == 13)
                        e->controller->inOut.speed = 2;
                    e->controller->inOut.distance = 32;
                    e->controller->inOut.timer = 0;
                    e->controller->inOut.basePosition = e->position;
                }
                else if(typeIndex == 63) // Chaining controllers
                {
                    e->controller->type = ControllerType_chain;
                }
            }
        }
        
    musicPlay(assetsGetMusic(&iw->game, "forestMusic"), 0.7, &iw->game);
    
    for(int t = 0; t < iw->level.entities.height; ++t)
        for(int i = 0; i < iw->level.entities.width; ++i)
        {
            Entity* e = iw->level.entityMap[i][t];
            if(e && e->controller->type == ControllerType_chain)
                resolveChain(e, iw);
        }
}

void saveMap(Iwbtg* iw, char* file)
{
    FILE* f = fopen(file, "w");
    fwrite(iw->level.entities.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
    fwrite(iw->level.controllers.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
    fclose(f);
}

void loadGame(Iwbtg* iw)
{
    Player* p = &iw->player;
    
    char saveFileName[128];
    snprintf(saveFileName, 128, "save%d.sav", iw->activeSaveSlot);
    
    iw->saveState.room.x = -1;
    iw->saveState.room.y = 0;
    
    FILE* f;
    if(f = fopen(saveFileName, "r"))
    {
        fread(&iw->saveState, sizeof(SaveState), 1, f);
        fclose(f);
    }
    
    p->dead = false;
    iw->room = iw->saveState.room;
    p->velocity.x = p->velocity.y = 0;
    iw->state = GameState_inGame;
    iw->time = 0;
    
    char buffer[128];
    loadMap(iw, getCurrentMapName(iw, buffer, 128));
    
    p->position = iw->saveState.playerPosition;
}

void saveGame(Iwbtg* iw, bool position)
{
    if(position)
    {
        iw->saveState.playerPosition = iw->player.position;
        iw->saveState.room = iw->room;
    }
    
    char saveFileName[128];
    snprintf(saveFileName, 128, "save%d.sav", iw->activeSaveSlot);
    
    // Save
    FILE* f = fopen(saveFileName, "w");
    fwrite(&iw->saveState, sizeof(SaveState), 1, f);
    fclose(f);
}


bool entityCheckPlayerCollision(Entity* e, Player* p)
{
    if(e->active)
        return checkRectangleIntersectSprite(&p->hitBox, &p->position, &e->sprite, &e->position);
}

bool rectangleCheckCollision(Rectanglef* r, Iwbtg* iw, EntityType type)
{
    Vector2f offset = { 0.0f, 0.0f };
    
    for(int i = 0; i < MAX_ENTITIES; ++i)
    {
        Entity* e = &iw->entities[i];
        if(e->active && e->type == type)
        {
            if(checkRectangleIntersectSprite(r, &offset, &e->sprite, &e->position))
                return true;
        }
    }
    
    return false;
}

Entity* playerCheckCollision(Player* p, Iwbtg* iw, EntityType type)
{   
    for(int i = 0; i < MAX_ENTITIES; ++i)
    {
        Entity* e = &iw->entities[i];
        if(e->active && e->type == type)
        {
            if(checkRectangleIntersectSprite(&p->hitBox, &p->position, &e->sprite, &e->position))
                return e;
        }
    }
    
    return 0;
}

Entity* playerCheckCollisionAtOffset(Player* p, Iwbtg* iw, EntityType type, int x, int y)
{   
    Vector2f pos = p->position;
    p->position.x += x;
    p->position.y += y;
    
    Entity* result = playerCheckCollision(p, iw, type);
    
    p->position = pos;
    
    return result;
}

bool rectangleIsCollidingWithGround(Rectanglef* r, Iwbtg* iw, float offsetX, float offsetY)
{
    Rectanglef block = { 0.0f, 0.0f, 32.0f, 32.0f };
    
    int px = (int)(offsetX);
    int py = (int)(offsetY);
    
    int fx = (px / 32) - 1;
    int fy = (py / 32) - 1;
    int tx = (px / 32) + 2;
    int ty = (py / 32) + 2;
    
    fx = clampi(fx, 0, iw->level.ground.width);
    fy = clampi(fy, 0, iw->level.ground.height);
    tx = clampi(tx, 0, iw->level.ground.width);
    ty = clampi(ty, 0, iw->level.ground.height);
    
    for(int i = fx; i < tx; ++i)
        for(int t = fy; t < ty; ++t)
            if(iw->level.ground.data[i + (t * iw->level.ground.width)] == 1
            && rectanglefIntersectAt(offsetX, offsetY, r,
                                     i * 32, t * 32, &block))
                return true;
        
    return false;
}

bool playerIsCollidingWithGround(Player* p, Iwbtg* iw, float offsetX, float offsetY)
{
    return rectangleIsCollidingWithGround(&p->hitBox, iw, p->position.x + offsetX, p->position.y + offsetY)
    || playerCheckCollisionAtOffset(p, iw, EntityType_movingPlatform, offsetX, offsetY);
}

void playerInit(Player* p, float x, float y, Iwbtg* iw)
{
    p->position.x = x;
    p->position.y = y;
    
    p->hitBox.x = 10.0f;
    p->hitBox.y = 10.0f;
    p->hitBox.w = 13.0f; 
    p->hitBox.h = 22.0f;
    
    spriteInit(&p->sprite, assetsGetTexture(&iw->game, "kid"), 32, 32);
    
    int stand[] = {0, 1, 2, 3};
    int run[] = {4, 5, 6, 7};
    int jump[] = {10, 11};
    int fall[] = {12, 13};
    int slide[] = {14, 15};
    spriteAddAnimation(&p->sprite, Animations_stand, &stand[0], 4, 6);
    spriteAddAnimation(&p->sprite, Animations_run, &run[0], 4, 16);
    spriteAddAnimation(&p->sprite, Animations_jump, &jump[0], 2, 12);
    spriteAddAnimation(&p->sprite, Animations_fall, &fall[0], 2, 12);
    spriteAddAnimation(&p->sprite, Animations_slide, &slide[0], 2, 12);
    
    spritePlayAnimation(&p->sprite, Animations_stand);
    
    p->runSpeed = 3;
    p->gravity = 0.4;
    p->maxFallSpeed = 9;
    p->jumpSpeed = 8.5;
    p->doubleJumpSpeed = 7;
    p->jumpDampening = 0.45;
    p->doubleJumpAvailible = true;
}

void playerUpdate(Player* p, Iwbtg* iw)
{
    Game* g = &iw->game;
    
    if(!p->dead)
    {
        // Player die
		
		// Hack / epsilon so that player can stand on a platform with
		// a spike inside it.
		float ty = p->position.y;
		p->position.y = floor(p->position.y) - 0.01;
		
        if(playerCheckCollision(p, iw, EntityType_spike) || playerCheckCollision(p, iw, EntityType_fruit))
        {
            for(int t = 1; t <= 8; ++t)
                for(int i = 0; i < 16; ++i)
                {
                    float a = ((float)i + ((float)t / 2) ) * PI * 0.25 * 0.5;
                    float s = t;
                    s *= s * 0.2;
                    Entity* pa = createParticle(iw, assetsGetTexture(&iw->game, "playerDeathParticle"), 
                                                p->position.x, p->position.y, cos(a)*s, sin(a)*s, 3);
                    pa->sprite.scale.x = 0.1 + (min(((1.0f / s) * 2), 1.5));
                    pa->sprite.scale.y = 0.1 + (min(((1.0f / s) * 2), 1.5));
                }
            musicPause(&iw->game);
            soundPlay(assetsGetSound(&iw->game, "death"), 1);
            
            p->dead = true;
            iw->state = GameState_gameOver;
            iw->gameOverTimer = 0;
        }
		p->position.y = ty;
        
        
        
        // Ground check
        bool onGround = playerIsCollidingWithGround(p, iw, 0, 1);
        p->velocity.x = 0;
        
        // Jumping
        if(onGround)
        {
            p->doubleJumpAvailible = true;
            if(checkKeyPressed(g, KEY_JUMP))
            {
                soundPlay(assetsGetSound(&iw->game, "jump"), 1);
                p->velocity.y = -p->jumpSpeed;
            }
        }
        else
        {
            if(checkKeyPressed(g, KEY_JUMP) && p->doubleJumpAvailible)
            {
                p->velocity.y = -p->doubleJumpSpeed;
                p->doubleJumpAvailible = false;
                soundPlay(assetsGetSound(&iw->game, "doubleJump"), 1);
            }
        }

        if(checkKeyReleased(g, KEY_JUMP))
			if(p->velocity.y < 0)
				p->velocity.y *= p->jumpDampening;
        
        // Gravity
        p->velocity.y += p->gravity;
        if(p->velocity.y > p->maxFallSpeed)
            p->velocity.y = p->maxFallSpeed;
        
        if(checkKeyPressed(g, KEY_SHOOT))
        {
            if(entityCount(iw, EntityType_playerBullet) < 6)
            {
                Entity* b = createEntity(iw, EntityType_playerBullet, p->position.x + 16, p->position.y + 19);
                b->velocity.x = p->sprite.scale.x * 16;
                b->position.x += p->sprite.scale.x * 6;
                soundPlay(assetsGetSound(&iw->game, "shoot"), 1);
            }
        }
        
        // Running
        if(checkKey(g, KEY_LEFT))
        {
            p->sprite.scale.x = -1;
            p->velocity.x -= 3;
        }
        
        if(checkKey(g, KEY_RIGHT))
        {
            p->sprite.scale.x = 1;
            p->velocity.x += 3;
        }
        
        // Perform sprite animation logic
        if(onGround)
        {
            if(p->velocity.x == 0)
                spritePlayAnimation(&p->sprite, Animations_stand);
            else
                spritePlayAnimation(&p->sprite, Animations_run);
        }
        else
        {
            if(p->velocity.y >= 0)
                spritePlayAnimation(&p->sprite, Animations_fall);
            else
                spritePlayAnimation(&p->sprite, Animations_jump);
        }
        
        // Move the player
        p->position.x += p->velocity.x;
        if(playerIsCollidingWithGround(p, iw, 0, 0))
        {
            for(int i = 0; i <= abs(p->velocity.x) + 1; ++i)
            {
                if(!playerIsCollidingWithGround(p, iw, 0, 0))
                    break;
                p->position.x -= sign(p->velocity.x);
            }
            p->velocity.x = 0;
        }
        
        p->position.y += p->velocity.y;
        if(playerIsCollidingWithGround(p, iw, 0, 0))
        {
            if(p->velocity.y > 0)
                p->position.y = floorf(p->position.y);
            if(p->velocity.y < 0)
                p->position.y = ceilf(p->position.y);
            
            for(int i = 0; i <= abs(p->velocity.y) + 1; ++i)
            {
                if(!playerIsCollidingWithGround(p, iw, 0, 0))
                    break;
                p->position.y -= sign(p->velocity.y);
            }
            p->velocity.y = 0;
        }
        
        // Warp the player if at the edge of a screen
        unsigned char warped = 0;
        
        if(p->position.x < -16)
        {
            p->position.x = iw->game.size.x - 16;
            warped = 1;
            iw->room.x--;
        }
        
        if(p->position.y < -16)
        {
            p->position.y = iw->game.size.y - 16;
            warped = 1;
            iw->room.y--;
        }
        
        if(p->position.x > iw->game.size.x - 16)
        {
            p->position.x = -16;
            warped = 1;
            iw->room.x++;
        }
        
        if(p->position.y > iw->game.size.y - 16)
        {
            p->position.y = -16;
            warped = 1;
            iw->room.y++;
        }
        
        if(warped)
        {
            char buffer[128];
            Vector2f playerPosition = p->position;
            loadMap(iw, getCurrentMapName(iw, buffer, 128));
            p->position = playerPosition;
        }
    }
    
	Entity* warp;
    if(warp = playerCheckCollision(p, iw, EntityType_warp))
    {
        iw->room.y--;
        char buffer[128];
        loadMap(iw, getCurrentMapName(iw, buffer, 128));
        p->position.x = warp->position.x + 64 - 16;
		p->position.y = warp->position.y + 64 - 16;
    }
    
    if(checkKeyPressed(g, KEY_RESTART))
    {
        loadGame(iw);
    }

    // Update the player sprite
    spriteUpdate(&p->sprite, 1.0f / 50.0f);
}

void menuFunctionStart(MenuItem* mi, void* data)
{
    Iwbtg* iw = (Iwbtg*)data;
    iw->activeMenu = &iw->loadMenu;
}

void menuFunctionQuit(MenuItem* mi, void* data)
{
    Iwbtg* iw = (Iwbtg*)data;
    iw->game.running = false;
}

void menuFunctionLoadSave(MenuItem* mi, void* data)
{
    Iwbtg* iw = (Iwbtg*)data;
    iw->activeSaveSlot = mi->id + 1;
    iw->state = GameState_inGame;
    loadGame(iw);
}

void iwbtgInit(Iwbtg* iw)
{
    fontInit(&iw->game.font, assetsGetTexture(&iw->game, "font"), 24, 32, "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890,.!?\"'/\\<>()=:");
    fontSetAllLetterWidth(&iw->game.font, 20);
    
    editorInit(iw);
    
    iw->blockTexture = assetsGetTexture(&iw->game, "block");
    iw->backgroundTexture = assetsGetTexture(&iw->game, "background");
    iw->cloudsTexture = assetsGetTexture(&iw->game, "clouds");
    iw->titleTexture = assetsGetTexture(&iw->game, "title");
    
    MenuItem* mi;
    
    iw->frameCount = 0;
    
    // MAIN MENU
    menuInit(&iw->mainMenu, iw->game.size.x / 2, 540 - 230, iw);
    iw->mainMenu.spacing.y = 15;
    
    mi = menuAddItem(&iw->mainMenu, MenuItemType_button, "START", iw);
    mi->function = menuFunctionStart;
    mi->functionData = (void*) iw;
    
    menuAddItem(&iw->mainMenu, MenuItemType_button, "OPTIONS", iw);
    
    mi = menuAddItem(&iw->mainMenu, MenuItemType_button, "QUIT", iw);
    mi->function = menuFunctionQuit;
    mi->functionData = (void*) iw;
    
    // SAVE MENU
    menuInit(&iw->loadMenu, 240, 540 - 180, iw);
    iw->loadMenu.spacing.x = 80;
    iw->loadMenu.orientation = MenuOrientation_horizontal;
    
    mi = menuAddItem(&iw->loadMenu, MenuItemType_button, "SLOT 1", iw);
    mi->function = menuFunctionLoadSave;
    mi->functionData = (void*) iw;
    
    mi = menuAddItem(&iw->loadMenu, MenuItemType_button, "SLOT 2", iw);
    mi->function = menuFunctionLoadSave;
    mi->functionData = (void*) iw;
    
    mi = menuAddItem(&iw->loadMenu, MenuItemType_button, "SLOT 3", iw);
    mi->function = menuFunctionLoadSave;
    mi->functionData = (void*) iw;
    
    iw->activeMenu = &iw->mainMenu;
    
    Texture* gameOverTexture = assetsGetTexture(&iw->game, "gameOver");
    spriteInit(&iw->gameOverSprite, assetsGetTexture(&iw->game, "gameOver"), 
               gameOverTexture->size.x, gameOverTexture->size.y);
    
    playerInit(&iw->player, 64, 128 + 32, iw);
    iw->saveState.playerPosition = iw->player.position;
    Vector2i startingRoom = {-1, 0};
    iw->room = iw->saveState.room = startingRoom;
    
    musicPlayOnce(assetsGetMusic(&iw->game, "menuMusic"), 0.5, &iw->game);
}

void iwbtgLoad(Iwbtg* iw)
{
    Game* g = &iw->game;
    assetsLoadTexture(g, "assets/kid.png", "kid"); //kid_silhouette
    assetsLoadTexture(g, "assets/block.png", "block");
    assetsLoadTexture(g, "assets/background.png", "background");
    assetsLoadTexture(g, "assets/clouds.png", "clouds");
    assetsLoadTexture(g, "assets/objects.png", "objects");
    assetsLoadTexture(g, "assets/spike.png", "spike");
    assetsLoadTexture(g, "assets/save.png", "save");
    assetsLoadTexture(g, "assets/player_death_particle.png", "playerDeathParticle");
    assetsLoadTexture(g, "assets/game_over.png", "gameOver");
    assetsLoadTexture(g, "assets/warp.png", "warp");
    assetsLoadTexture(g, "assets/block_tiles.png", "blockTiles");
    assetsLoadTexture(g, "assets/player_bullet.png", "playerBullet");
    assetsLoadTexture(g, "assets/font.png", "font");
    assetsLoadTexture(g, "assets/title.png", "title");
    assetsLoadTexture(g, "assets/fruit.png", "fruit");
    assetsLoadTexture(g, "assets/landscape.png", "landscape");
    assetsLoadTexture(g, "assets/controllers.png", "controllers");
    assetsLoadTexture(g, "assets/moving_platform.png", "movingPlatform");
    
    assetsLoadSound(g, "assets/jump.wav", "jump");
    assetsLoadSound(g, "assets/double_jump.wav", "doubleJump");
    assetsLoadSound(g, "assets/shoot.wav", "shoot");
    assetsLoadSound(g, "assets/death.wav", "death");
    assetsLoadSound(g, "assets/trap.wav", "trap");
    
    assetsLoadMusic(g, "assets/forest_music.ogg", "forestMusic");
    assetsLoadMusic(g, "assets/menu_music.ogg", "menuMusic");
}

/*chainControllerActivation(Iwbtg* iw, int x, int y, Controller* c)
{
    int x = 
}*/

void controllerUpdate(Controller* c, Entity* e, Iwbtg* iw, float dt)
{
    switch(c->type)
    {
        case ControllerType_trap: {
        
            ControllerTrap* trap = &e->controller->trap;
            
            
            float dx = cosf(trap->direction);
            float dy = -sinf(trap->direction);
            
            // Only check for activation on the owner of the controller
            if(c == &e->controllerData && !trap->activated)
            {
                Vector2f pos = vector2fAdd(e->position, vector2fDivide(vector2iTof(e->sprite.size), 2));
                Vector2f playerPos = {
                    iw->player.position.x + (iw->player.sprite.size.x / 2),
                    iw->player.position.y + (iw->player.sprite.size.y / 2)
                };
                
                Vector2f activationRange = {
                    dx * trap->activationDistance, 
                    dy * trap->activationDistance
                };
                
                Vector2f entityToPlayer = {
                    playerPos.x - pos.x,
                    playerPos.y - pos.y
                };
                
                Vector2f a = vector2fNormalize(activationRange);
                Vector2f b = vector2fDivide(entityToPlayer, trap->activationDistance);
                
                float dot = vector2fDot(a, b);
                Vector2f playerOnActivation = vector2fAdd(vector2fMultiply(activationRange, dot), pos);
                            
                if(dot >= 0 && dot <= 1 && vector2fDistance(playerOnActivation, playerPos) < (trap->activationWidth / 2))
                {
                    trap->activated = true;
                    soundPlay(assetsGetSound(&iw->game, "trap"), 1);
                    trap->activationFrame = iw->frameCount;
                }
            }

            if(trap->activated && (trap->activationFrame < iw->frameCount || !c->hasChains))
            {
                e->velocity.x = trap->speed * dx;
                e->velocity.y = trap->speed * dy;
            }
        } break;
        
        case ControllerType_inOut: {
            
            ControllerInOut* io = &e->controller->inOut;
            
            io->timer += dt * io->speed;
            float phase = fabs(sinf(io->timer));
            float dx = cosf(io->direction);
            float dy = -sinf(io->direction);
            
            e->position.x = io->basePosition.x + (dx * phase * io->distance) - (dx * io->distance);
            e->position.y = io->basePosition.y + (dy * phase * io->distance) - (dy * io->distance);
            
        } break;
    }
}

void iwbtgUpdate(Iwbtg* iw)
{
    Game* g = &iw->game;
    float dt = (float)1 / 50;
    iw->time += dt;
    
    iw->frameCount++;
    
    if(checkKeyPressed(g, KEY_FULLSCREEN_TOGGLE))
        gameFullscreenToggle(g);
    
    if(iw->state == GameState_menu)
    {
        updateMenu(iw->activeMenu, iw, dt);
        if(checkKeyPressed(g, KEY_MENU))
        {
            if(iw->activeMenu != &iw->mainMenu)
                iw->activeMenu = &iw->mainMenu;
            else
                iw->game.running = false;
        }
    } else if(iw->state == GameState_inGame || iw->state == GameState_gameOver)
    {
        if(iw->state == GameState_gameOver)
        {
            iw->gameOverTimer += dt;
        }
        
        if(!iw->editor.enabled)
            playerUpdate(&iw->player, iw);
        
        editorUpdate(iw);
        
        if(checkKeyPressed(g, KEY_SAVE_LEVEL))
        {
            char buffer[128];
            saveMap(iw, getCurrentMapName(iw, buffer, 128));
        }
        
        if(checkKeyPressed(g, KEY_LOAD_LEVEL))
        {
            char buffer[128];
            loadMap(iw, getCurrentMapName(iw, buffer, 128));
        }
        
        if(checkKeyPressed(g, KEY_MENU))
        {
            iw->state = GameState_menu;
            iw->activeMenu = &iw->mainMenu;
            musicStop(&iw->game);
            musicPlayOnce(assetsGetMusic(&iw->game, "menuMusic"), 0.5, &iw->game);
        }
        
        iw->entityDrawCount = 0;
        if(!iw->editor.enabled)
        {
            for(int i = 0; i < MAX_ENTITIES; ++i)
            {
                Entity* e = &iw->entities[i];
                if(e->active)
                {
                    iw->entityDrawOrder[iw->entityDrawCount++] = e;
                    e->velocity.x += e->acceleration.x;
                    e->velocity.y += e->acceleration.y;
                    e->position.x += e->velocity.x;
                    e->position.y += e->velocity.y;
                    spriteUpdate(&e->sprite, 1.0f / 50.0f);
                    
                    float dist = vector2fMagnitude(e->velocity);
                    dist -= e->friction * dt;
                    if(dist < 0)
                        dist = 0;
                    e->velocity = vector2fMultiply(vector2fNormalize(e->velocity), dist);
                    
                    e->sprite.angle += e->spin * dt * PI * 2;
                    
                    if((float)sign(e->spin) * e->spin > 0)
                    {
                        int s = sign(e->spin);
                        e->spin -= (float)sign(e->spin) * e->spinFriction  * dt * PI * 2;
                        if(s != sign(e->spin))
                            e->spin = 0;
                    }
                    
                    switch(e->type)
                    {
                        case EntityType_save:
                            
                            e->position.y -= sin(iw->time * 2) * 0.2;
                            
                            if(entityCheckPlayerCollision(e, &iw->player))
                            {
                                if(e->animationTimer <= 0)
                                    saveGame(iw, true);
                                e->sprite.frame = 1;
                                e->animationTimer = 0.5;
                            }
                            if(e->animationTimer > 0)
                            {
                                e->animationTimer -= dt;
                                if(e->animationTimer <= 0)
                                    e->sprite.frame = 0;
                            }
                            
                            break;
                            
                        case EntityType_particle:
                            
                            e->animationTimer -= dt;
                            e->sprite.alpha = e->animationTimer / 3;
                            if(e->animationTimer < 0)
                                destroyEntity(e);
                            
                            break;
                            
                        case EntityType_playerBullet:
                        
                            if(e->position.x > iw->game.size.x || e->position.y > iw->game.size.y
                            || e->position.x < -4 || e->position.y < -4)
                                destroyEntity(e);
                            
                            Rectanglef hitbox = { 0, 0, 4, 4 };
                            
                            if(rectangleIsCollidingWithGround(&hitbox, iw, e->position.x, e->position.y))
                                destroyEntity(e);
                            
                            break;
                            
                        case EntityType_fruit:
                            //e->sprite.angle += 0.01 * PI;
                            break;
                    }
                    
                    controllerUpdate(e->controller, e, iw, dt);
                }
            }
        }
    }
}

int compareEntitiesByDepth(const void* a, const void* b)
{
    Entity* ea = *((Entity**)a);
    Entity* eb = *((Entity**)b);
    
    return (ea->depth < eb->depth) - (ea->depth > eb->depth);
}

bool checkAdjacentBlock(Iwbtg* iw, Entity* e, int x, int y)
{
    int sx = ((int)e->position.x / 32) + x;
    int sy = ((int)e->position.y / 32) + y;
    
    int value = gridGet(&iw->level.entities, sx, sy);
    
    if(value == -1 || value == 1)
        return true;
    return false;
}

void iwbtgDraw(Iwbtg* iw)
{
    Game* g = &iw->game;
    renderBegin(g);
    
    textureDraw(g, iw->backgroundTexture, 0, 0);
    
    int offset = (int)(iw->time * 15) % iw->cloudsTexture->size.x;
    textureDraw(g, iw->cloudsTexture, offset, 0);
    textureDraw(g, iw->cloudsTexture, offset - iw->cloudsTexture->size.x, 0);
    
    textureDraw(g, assetsGetTexture(&iw->game, "landscape"), 0, 0);
    
    if(iw->state == GameState_menu)
    {
        int ox = (iw->game.size.x / 2) - (iw->titleTexture->size.x / 2);
        int oy = 80;
        textureDraw(&iw->game, iw->titleTexture, ox, oy);
        
        drawMenu(iw->activeMenu, iw);
    }
    
    if(iw->state == GameState_inGame || iw->state == GameState_gameOver)
    {
        qsort(iw->entityDrawOrder, iw->entityDrawCount, sizeof(Entity*), compareEntitiesByDepth);
        
        if(!iw->editor.enabled)
            for(int i = 0; i < iw->entityDrawCount; ++i)
            {
                Entity* e = iw->entityDrawOrder[i];
                if(e->active)
                {
                    switch(e->type)
                    {
                        case EntityType_warp:
                            for(int t = 0; t < 5; ++t)
                            {
                                e->sprite.frame = t;
                                if(t > 2) // Warp cloud effect
                                {
                                    e->sprite.scale.x = e->sprite.scale.y = 1;
                                    e->sprite.additiveBlend = true;
                                    e->sprite.alpha = 0.2;
                                    e->sprite.angle = iw->time * 2;
                                    if(t == 4)
                                        e->sprite.angle *= -1;
                                }
                                else // The dials of the warp
                                {
                                    float scaleAnimation = sin(iw->time);
                                    scaleAnimation = (float)sign(scaleAnimation) * (scaleAnimation * scaleAnimation * scaleAnimation);
                                    e->sprite.scale.x = e->sprite.scale.y = 0.6 + (scaleAnimation * 0.2);
                                    e->sprite.additiveBlend = false;
                                    e->sprite.alpha = 1;
                                    e->sprite.angle = iw->time * 0.5;
                                    if(t == 1)
                                        e->sprite.angle *= -1;
                                    if(t == 2)
                                        e->sprite.angle *= 2;
                                }
                                spriteDraw(g, &e->sprite, e->position.x, e->position.y);
                            }
                            e->sprite.frame = 5; // Collision Mask
                            break;
                            
                        case EntityType_block: {
                            Sprite* s = &e->sprite;
                            
                            int tr, tl, br, bl;
                            
                            // Code ripped and translated directly from an
                            // old project. Probably needs some tidying.
                            {
                                int bn = checkAdjacentBlock(iw, e, 0, -1);
                                int bne = checkAdjacentBlock(iw, e, 1, -1);
                                int bnw = checkAdjacentBlock(iw, e, -1, -1);

                                int bs = checkAdjacentBlock(iw, e, 0, 1);
                                int bse = checkAdjacentBlock(iw, e, 1, 1);
                                int bsw = checkAdjacentBlock(iw, e, -1, 1);

                                int be = checkAdjacentBlock(iw, e, 1, 0);
                                int bw = checkAdjacentBlock(iw, e, -1, 0);

                                // tl
                                {
                                    int tx = 0, 
                                        ty = 0;
                                    if(bn) ty += 1;
                                    if(bw) tx += 1;
                                    if(bn && bw && !bnw) { tx = 4; ty = 3; }
                                    tl = tx + (ty*5);
                                }

                                // tr
                                {
                                    int tx = 3, 
                                        ty = 0;
                                    if(bn) ty += 1;
                                    if(be) tx -= 1;
                                    if(bn && be && !bne) { tx = 4; ty = 2; }
                                    tr = tx + (ty*5);
                                }

                                // bl
                                {
                                    int tx = 0, 
                                        ty = 3;
                                    if(bs) ty -= 1;
                                    if(bw) tx += 1;
                                    if(bs && bw && !bsw) { tx = 4; ty = 1; }
                                    bl = tx + (ty*5);
                                }

                                // br
                                {
                                    int tx = 3, 
                                        ty = 3;
                                    if(bs) ty -= 1;
                                    if(be) tx -= 1;
                                    if(bs && be && !bse) { tx = 4; ty = 0; }
                                    br = tx + (ty*5);
                                }
                            }
                            
                            s->frame = tl;
                            spriteDraw(g, s, e->position.x, e->position.y);
                            s->frame = tr;
                            spriteDraw(g, s, e->position.x + 16, e->position.y);
                            s->frame = bl;
                            spriteDraw(g, s, e->position.x, e->position.y + 16);
                            s->frame = br;
                            spriteDraw(g, s, e->position.x + 16, e->position.y + 16);
                        } break;
                          
                        default:
                            spriteDraw(g, &e->sprite, e->position.x, e->position.y);
                            
                            //Rectanglef r = {0.0f, 0.0f, 64.0f, 64.0f};
                            //checkRectangleIntersectSpriteDraw(&r, &e->position, &e->sprite, &e->position, iw);
                            break;
                    }
                }
            }

        Player* p = &iw->player;
        if(!p->dead)
            spriteDraw(g, &iw->player.sprite, iw->player.position.x, iw->player.position.y);
        
        char roomText[128];
        snprintf(roomText, 128, "ROOM %d,%d", iw->room.x, iw->room.y);
        drawText(&iw->game, 0, roomText, 8, 8);
        
        editorDraw(iw);
        
        if(iw->state == GameState_gameOver)
        {
            int cx = (g->size.x - iw->gameOverSprite.texture->size.x) / 2;
            int cy = (g->size.y - iw->gameOverSprite.texture->size.y) / 2;
            
            Sprite* s = &iw->gameOverSprite;
            
            if(iw->gameOverTimer > 0.5)
            {
                float t = iw->gameOverTimer - 0.5;
                //t *= t;
                s->alpha = t * 4;
                s->scale.x = max(4 - (t*16), 1);  
                s->scale.y = max(2 - (t*4), 1);  
                
                rectangleDraw(g, 0, 0, g->size.x, g->size.y, 0.15, 0.15, 0.3, t * 0.3);
                spriteDraw(g, s, cx, cy);
            }
        }
        
        rectangleDraw(g, iw->debugDrawPosition.x, iw->debugDrawPosition.y, 4, 4, 1, 0, 1, 1);
    }
    
    renderEnd(g);
}

#undef main
int main(int argc, char** argv)
{
    Iwbtg* iwbtg = new(Iwbtg);
    Game* game = &iwbtg->game;
    
    gameInit(game, "iwbtg", 960, 540, 1);
    
    iwbtgLoad(iwbtg);
    iwbtgInit(iwbtg);
    
    //loadMap(iwbtg, "assets/1.map");
    
    while(iwbtg->game.running)
    {
        double frameTime = (double)1000.0 / 50.0;
        int updateTime = SDL_GetTicks();
        game->frameTimer += updateTime - game->lastUpdateTime;
        
        while(game->frameTimer >= frameTime)
        {
            gameHandleEvents(&iwbtg->game);
            iwbtgUpdate(iwbtg);
            game->frameTimer -= frameTime;
        }
        iwbtgDraw(iwbtg);
        
        memoryPoolClear(&game->frameMemory);
	    game->lastUpdateTime = updateTime;
    }
}