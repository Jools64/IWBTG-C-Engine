#define PI 3.141592
#include "engine/engine.h"
#include "menu.h"

// TODO: Stop being so lazy and break up this into headers and c source files.

// HEADER

int defaultMap[] = {
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

typedef struct
{
    int width, height;
    int* data;
} Grid;

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
} EntityType;

typedef struct
{
    Vector2f position, velocity, acceleration;
    Sprite sprite;
    EntityType type;
    bool active;
    float spin, spinFriction, friction;
    float animationTimer;
    float depth;
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

typedef struct
{
    bool enabled;
    int mode;
    int selectedObject;
    Grid* editLayer;
    Sprite objectSprite;
} Editor;

typedef enum
{
    GameState_menu,
    GameState_inGame,
    GameState_gameOver
} GameState;

typedef struct
{
    Vector2f playerPosition;
    int room;
    int deaths;
    double time;
} SaveState;

#define MAX_ENTITIES 1024
#define MAP_WIDTH 30
#define MAP_HEIGHT 17

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
    Texture* objectsTexture;
    Texture* gameOverTexture;
    Texture* titleTexture;
    
    Sprite gameOverSprite;
    
    Grid map;
    Editor editor;
    
    Menu mainMenu, loadMenu;
    Menu* activeMenu;
    
    Entity entities[MAX_ENTITIES];
    Entity* entityDrawOrder[MAX_ENTITIES];
    int lastEntityPosition, entityDrawCount, room,
        activeSaveSlot;
    float time;
    int mapDataMemory[MAP_WIDTH * MAP_HEIGHT];
    
    GameState state;
    float gameOverTimer;
} Iwbtg;

// C FILES

#include "menu.c"

// IMPLEMENTATION

void gridSet(Grid* g, int x, int y, int value)
{
    if(x >= 0 && y >= 0 && x < g->width && y < g->height)
        g->data[x + (y * g->width)] = value;
}

int gridGet(Grid* g, int x, int y)
{
    if(x >= 0 && y >= 0 && x < g->width && y < g->height)
        return g->data[x + (y * g->width)];
    return -1;
}

char* getCurrentMapName(Iwbtg* iw, char* string, int stringLength)
{
    snprintf(string, stringLength, "assets/%d.map", iw->room);
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
    
    // Important: All entities must have a sprite. (Or I need to implement behavior to handle this)
    switch(type)
    {
        case EntityType_block:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "blockTiles"), 16, 16);
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

void loadMap(Iwbtg* iw, char* file)
{
    FILE* f;
    if(f = fopen(file, "r"))
    {
        fread(iw->map.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
        fclose(f);
    }
    else
    {
        memcpy(iw->map.data, defaultMap, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
    }
    
    destroyAllEntities(iw);
    
    for(int i = 0; i < iw->map.width; ++i)
        for(int t = 0; t < iw->map.height; ++t)
        {
            int typeIndex = iw->map.data[i + (t * iw->map.width)] - 1;
            
            int x = 32 * i,
                y = 32 * t;
            
            
            if(typeIndex == 0)
                createEntity(iw, EntityType_block, x, y);
            else if(typeIndex >= 2 && typeIndex <= 5)
            {
                Entity* e = createEntity(iw, EntityType_spike, x, y);
                e->sprite.frame = typeIndex - 2;
            }
            else if(typeIndex == 6)
                createEntity(iw, EntityType_fruit, x, y);
            else if(typeIndex == 10)
                createEntity(iw, EntityType_save, x, y);
            else if(typeIndex == 11)
                createEntity(iw, EntityType_warp, x-48, y-48);
            else if(typeIndex == 1)
            {
                iw->player.position.x = x;
                iw->player.position.y = y;
            }
                
        }
}

void saveMap(Iwbtg* iw, char* file)
{
    FILE* f = fopen(file, "w");
    fwrite(iw->map.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
    fclose(f);
}

void loadGame(Iwbtg* iw)
{
    Player* p = &iw->player;
    
    char saveFileName[128];
    snprintf(saveFileName, 128, "save%d.sav", iw->activeSaveSlot);
    
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

bool rectangleIsCollidingWithGround(Rectanglef* r, Iwbtg* iw, float offsetX, float offsetY)
{
    Rectanglef block = { 0.0f, 0.0f, 32.0f, 32.0f };
    
    int px = (int)(offsetX);
    int py = (int)(offsetY);
    
    int fx = (px / 32) - 1;
    int fy = (py / 32) - 1;
    int tx = (px / 32) + 2;
    int ty = (py / 32) + 2;
    
    fx = clampi(fx, 0, iw->map.width);
    fy = clampi(fy, 0, iw->map.height);
    tx = clampi(tx, 0, iw->map.width);
    ty = clampi(ty, 0, iw->map.height);
    
    for(int i = fx; i < tx; ++i)
        for(int t = fy; t < ty; ++t)
            if(iw->map.data[i + (t * iw->map.width)] == 1
            && rectanglefIntersectAt(offsetX, offsetY, r,
                                     i * 32, t * 32, &block))
                return true;
        
    return false;
}

bool playerIsCollidingWithGround(Player* p, Iwbtg* iw, float offsetX, float offsetY)
{
    return rectangleIsCollidingWithGround(&p->hitBox, iw, p->position.x + offsetX, p->position.y + offsetY);
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
            
            p->dead = true;
            iw->state = GameState_gameOver;
            iw->gameOverTimer = 0;
        }
		p->position.y = ty;
        
        // Gravity
        p->velocity.y += p->gravity;
        if(p->velocity.y > p->maxFallSpeed)
            p->velocity.y = p->maxFallSpeed;
        
        // Ground check
        bool onGround = playerIsCollidingWithGround(p, iw, 0, 1);
        p->velocity.x = 0;
        
        // Jumping
        if(onGround)
        {
            p->doubleJumpAvailible = true;
            if(checkKeyPressed(g, KEY_JUMP))
                p->velocity.y = -p->jumpSpeed;
        }
        else
        {
            if(checkKeyPressed(g, KEY_JUMP) && p->doubleJumpAvailible)
            {
                p->velocity.y = -p->doubleJumpSpeed;
                p->doubleJumpAvailible = false;
            }
        }

        if(checkKeyReleased(g, KEY_JUMP))
			if(p->velocity.y < 0)
				p->velocity.y *= p->jumpDampening;
        
        if(checkKeyPressed(g, KEY_SHOOT))
        {
            if(entityCount(iw, EntityType_playerBullet) < 6)
            {
                Entity* b = createEntity(iw, EntityType_playerBullet, p->position.x + 16, p->position.y + 19);
                b->velocity.x = p->sprite.scale.x * 16;
                b->position.x += p->sprite.scale.x * 6;
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
    }
    
	Entity* warp;
    if(warp = playerCheckCollision(p, iw, EntityType_warp))
    {
        iw->room++;
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
    
    iw->map.width = MAP_WIDTH;
    iw->map.height = MAP_HEIGHT;
    iw->map.data = iw->mapDataMemory;
    iw->editor.selectedObject = 1;
    iw->state = GameState_menu;
    
    iw->blockTexture = assetsGetTexture(&iw->game, "block");
    iw->backgroundTexture = assetsGetTexture(&iw->game, "background");
    iw->cloudsTexture = assetsGetTexture(&iw->game, "clouds");
    iw->objectsTexture = assetsGetTexture(&iw->game, "objects");
    iw->titleTexture = assetsGetTexture(&iw->game, "title");
    
    MenuItem* mi;
    
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
    
    spriteInit(&iw->editor.objectSprite, iw->objectsTexture, 32, 32);
    
    Texture* gameOverTexture = assetsGetTexture(&iw->game, "gameOver");
    spriteInit(&iw->gameOverSprite, assetsGetTexture(&iw->game, "gameOver"), 
               gameOverTexture->size.x, gameOverTexture->size.y);
    
    playerInit(&iw->player, 64, 128 + 32, iw);
    iw->saveState.playerPosition = iw->player.position;
    iw->room = iw->saveState.room = 1;
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
}

void iwbtgUpdate(Iwbtg* iw)
{
    Game* g = &iw->game;
    float dt = (float)1 / 50;
    iw->time += dt;
    
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
        
        playerUpdate(&iw->player, iw);
        
        if(iw->editor.enabled)
        {
            int mx = iw->game.input.mousePosition.x / 32;
            int my = iw->game.input.mousePosition.y / 32;
                
            if(checkMouseButton(g, SDL_BUTTON_LEFT))
            {
                
                if(checkKey(g, KEY_TILE_PICKER))
                    iw->editor.selectedObject = mx + (my * (iw->objectsTexture->size.x / 32)) + 1;
                else
                    gridSet(&iw->map, mx, my, iw->editor.selectedObject);
            }
            else if(checkMouseButton(g, SDL_BUTTON_RIGHT))
                gridSet(&iw->map, mx, my, 0);
        }
        
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
        
        if(checkKeyPressed(g, KEY_EDITOR_TOGGLE))
        {
            iw->editor.enabled = !iw->editor.enabled;
            if(!iw->editor.enabled)
            {
                char buffer[128];
                saveMap(iw, getCurrentMapName(iw, buffer, 128));
                loadMap(iw, getCurrentMapName(iw, buffer, 128));
            }
        }
        
        if(checkKeyPressed(g, KEY_MENU))
        {
            iw->state = GameState_menu;
            iw->activeMenu = &iw->mainMenu;
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
    
    int value = gridGet(&iw->map, sx, sy);
    
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
                            break;
                    }
                }
            }

        Player* p = &iw->player;
        if(!p->dead)
            spriteDraw(g, &iw->player.sprite, iw->player.position.x, iw->player.position.y);
        
        if(iw->editor.enabled)
        {
            Sprite* objectSprite = &iw->editor.objectSprite; 
            
            for(int i = 0; i < iw->map.width; ++i)
                for(int t = 0; t < iw->map.height; ++t)
                {
                    Sprite* objectSprite = &iw->editor.objectSprite;
                    objectSprite->frame = iw->map.data[i + (t * iw->map.width)] - 1;
                    
                    bool collision = false;
                    
                    if(objectSprite->frame > 1)
                    {
                        Vector2f position = { (float)i * 32.0f, (float)t * 32.0f };
                        
                        if(checkRectangleIntersectSprite(&p->hitBox, &p->position, objectSprite, &position))
                            collision = true;
                    }
                    
                    if(objectSprite->frame >= 0 && !collision)
                        spriteDraw(g, objectSprite, i * 32, t * 32);
                }
            
            objectSprite->frame = iw->editor.selectedObject-1;
            spriteDraw(g, &iw->editor.objectSprite, 4, 4);
            
            if(checkKey(g, KEY_TILE_PICKER))
                textureDraw(g, iw->objectsTexture, 0, 0);
            
        }
        
        char roomText[128];
        snprintf(roomText, 128, "ROOM %d", iw->room);
        drawText(&iw->game, 0, roomText, 8, 8);
        
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
    
    loadMap(iwbtg, "assets/1.map");
    
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