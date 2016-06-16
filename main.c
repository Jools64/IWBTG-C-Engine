/*

    TODO:
    
    - Restart music at correct position after game over

*/

#define NO_MUSIC

#define PI 3.14159265359
#define MAX_ENTITIES 2048
#define MAP_WIDTH 30
#define MAP_HEIGHT 17
#define GRID_SIZE 32

#include <ctype.h>

#include "engine/engine.h"
#include "controller.h"
#include "entity.h"
#include "defaultMap.h"
#include "menu.h"
#include "editor.h"
#include "player.h"
#include "main.h"
#include "collision.h"

#include "script.c"
#include "menu.c"
#include "editor.c"
#include "controller.c"
#include "player.c"
#include "entity.c"

Script* levelGetScriptAtPosition(Level* l, int x, int y)
{
    for(int i = 0; i < MAX_SCRIPTS_PER_LEVEL; ++i)
        if(l->scripts[i].inUse && l->scripts[i].position.x == x 
           && l->scripts[i].position.y == y)
            return &l->scripts[i];
    return 0;
}

Script* levelAddScript(Level* l, int x, int y)
{
    for(int i = 0; i < MAX_SCRIPTS_PER_LEVEL; ++i)
        if(!l->scripts[i].inUse)
        {
            l->scripts[i].position.x = x;
            l->scripts[i].position.y = y;
            l->scripts[i].inUse = true;
            l->scripts[i].text[0] = '\0';
            return &l->scripts[i];
        }
    printf("Error: Out of room for scripts!");
    return 0;
}

void levelRemoveScript(Level* l, int x, int y)
{
    for(int i = 0; i < MAX_SCRIPTS_PER_LEVEL; ++i)
        if(l->scripts[i].inUse && l->scripts[i].position.x == x 
           && l->scripts[i].position.y == y)
           l->scripts[i].inUse = false;
}

void textInputEditString(TextInput* ti, char* string, int stringMaxLength)
{
    ti->text = string;
    ti->textMaxLength = stringMaxLength;
    ti->cursorPosition = strlen(string);
    ti->active = true;
}

void textInputInit(TextInput* ti)
{
    ti->text = 0;
    ti->textMaxLength = 0;
    ti->length = 0;
    ti->active = false;
    ti->position.x = 256;
    ti->position.y = 128;
    ti->backspaceFrames = 0;
    ti->leftFrames = 0;
    ti->rightFrames = 0;
    ti->cursorPosition = 0;
}

// TODO: Optimize and make safe
void stringInsert(char* destString, char* string, int position, int maxSize)
{
    char buffer[maxSize];
    
    strncpy(buffer, destString, position);
    buffer[position] = '\0';
    strcat(buffer, string);
    strcat(buffer, destString + position);
    strcpy(destString, buffer);
}

// TODO: Optimize and make safe
void stringRemoveCharacter(char* string, int position, int maxSize)
{
    char buffer[maxSize];
    
    strncpy(buffer, string, position - 1);
    buffer[position - 1] = '\0';
    strcat(buffer, string + position);
    strcpy(string, buffer);
}

void textInputUpdate(TextInput* ti, Iwbtg* iw)
{
    Game* g = &iw->game;
    
    if(ti->active)
    {
        // Force upper case on all input
        
        char* text = ti->text;
        
        int inputLength = strlen(g->input.text);
        if(inputLength + strlen(g->input.text) < ti->textMaxLength-1 && inputLength > 0)
        {
            stringInsert(text, g->input.text, ti->cursorPosition, SCRIPT_MAX_LENGTH);
            ti->cursorPosition += inputLength;
        }
           
        
        if(g->input.keysPressed[SDLK_LEFT & 255])
        {
            ti->cursorPosition--;
            ti->leftFrames = 0;
        }
        
        if(g->input.keys[SDLK_LEFT & 255])
        {
            ti->leftFrames++;
            if(ti->leftFrames > 25)
            {
                ti->cursorPosition--;
            }
        }
        
        if(g->input.keysPressed[SDLK_RIGHT & 255])
        {
            ti->cursorPosition++;
            ti->rightFrames = 0;
        }
        
        if(g->input.keys[SDLK_RIGHT & 255])
        {
            ti->rightFrames++;
            if(ti->rightFrames > 25)
            {
                ti->cursorPosition++;
            }
        }
           

        ti->cursorPosition = min(max(ti->cursorPosition, 0), strlen(text));
        
        if(g->input.keysPressed[SDLK_BACKSPACE & 255])
        {
            int length = strlen(text);
            if(length > 0)
            {
                stringRemoveCharacter(text, ti->cursorPosition, SCRIPT_MAX_LENGTH);
                ti->cursorPosition--;
            }
            ti->backspaceFrames = 0;
        }
        
        if(g->input.keys[SDLK_BACKSPACE & 255])
        {
            ti->backspaceFrames++;
            if(ti->backspaceFrames > 25)
            {
                int length = strlen(text);
                if(length > 0)
                {
                    stringRemoveCharacter(text, ti->cursorPosition, SCRIPT_MAX_LENGTH);
                    ti->cursorPosition--;
                }
            }
        }
        
        if(g->input.keysPressed[SDLK_RETURN & 255])
        {
            if(strlen("\n") + strlen(g->input.text) < ti->textMaxLength-1)
            {
                stringInsert(text, "\n", ti->cursorPosition, SCRIPT_MAX_LENGTH);
                ti->cursorPosition++;
            }
        }
        
        if(g->input.keysPressed[SDLK_ESCAPE & 255])
            ti->active = false;
        
        // Stop any input being interpreted by the game
        g->input.frameVoid = true;
    }
}

void textInputDraw(TextInput* ti, Iwbtg* iw)
{
    if(ti->active)
    {
        int padding = 8;
        Vector2f size = getTextSize(&iw->fontSmall, ti->text);
        
        ti->position.x = (iw->game.size.x / 2) - (size.x / 2) - padding;
        ti->position.y = (iw->game.size.y / 2) - (size.y / 2) - padding;
        
        rectangleDraw(&iw->game, ti->position.x - padding, ti->position.y - padding, 
                      size.x + (padding * 2), size.y + (padding * 2) - 2, 0.2, 0.1, 0.3, 0.8);
                      
        drawText(&iw->game, &iw->fontSmall, ti->text, ti->position.x, ti->position.y);
        
        Vector2f cursorPosition = getTextCharPosition(&iw->fontSmall, ti->text, ti->cursorPosition);
        
        rectangleDraw(&iw->game, ti->position.x + cursorPosition.x, ti->position.y + cursorPosition.y, 2, 16, 1.0, 1.0, 1.0, 1.0);
    }
}

char* getCurrentMapName(Iwbtg* iw, char* string, int stringLength)
{
    snprintf(string, stringLength, "assets/%dx%d.map", iw->room.x, iw->room.y);
    return string;
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
    e->depth = -10;

    spriteInit(&e->sprite, texture, texture->size.x, texture->size.y);
    e->sprite.additiveBlend = true;
    
    return e;
}

void loadRoomFile(Iwbtg* iw, char* name)
{
    char roomFile[255];
    snprintf(roomFile, 255, "assets/%s.rm", name);
    
    ScriptState roomInfoScript = loadAndParseScript(roomFile);
    
    if(scriptHasValue(&roomInfoScript, "blockTexture"))
    {
        Texture* blockTexture = assetsGetTexture(&iw->game, scriptReadString(&roomInfoScript, "blockTexture", "no block texture"));
        if(blockTexture)
            iw->blockTexture = blockTexture;
    }
    
    char buffer[128];
    for(int i = 0; i < MAX_BACKGROUNDS_PER_LEVEL; ++i)
    {
        snprintf(buffer, 128, "background%d", i);
        if(scriptHasValue(&roomInfoScript, buffer))
        {
            iw->backgrounds[i].texture = assetsGetTexture(&iw->game, scriptReadString(&roomInfoScript, buffer, "background name missing!?"));
            iw->backgrounds[i].enabled = true;
            
            snprintf(buffer, 128, "background%dhSpeed", i);
            iw->backgrounds[i].speed.x = (float)scriptReadNumber(&roomInfoScript, buffer, 0);snprintf(buffer, 128, "background%dvSpeed", i);
            iw->backgrounds[i].speed.y = (float)scriptReadNumber(&roomInfoScript, buffer, 0);
        }
    }
}

void loadMap(Iwbtg* iw, char* file)
{
    iw->boss = 0;
    
    Background bgClear = {0};
    for(int i = 0; i < MAX_BACKGROUNDS_PER_LEVEL; ++i)
        iw->backgrounds[i] = bgClear;
    
    FILE* f;
    if((f = fopen(file, "rb")))
    {
        fread(iw->level.entities.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
        fread(iw->level.controllers.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
        
        int scriptCount;
        fread(&scriptCount, sizeof(int), 1, f);
        
        for(int i = 0; i < MAX_SCRIPTS_PER_LEVEL; ++i)
        {
            iw->level.scripts[i].text[0] = '\0';
            iw->level.scripts[i].inUse = false;
        }
        
        for(int i = 0; i < scriptCount; ++i)
        {
            fread(&iw->level.scripts[i], sizeof(Script), 1, f);
        }
        
        fread(&iw->level.propertiesScript[0], MAX_LEVEL_PROPERTIES_SCRIPT_LENGTH, 1, f);
        
        memset(iw->level.ground.data, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->entityMap, 0, sizeof(Entity*) * MAP_WIDTH * MAP_HEIGHT); 
        fclose(f);
    }
    else
    {
        memcpy(iw->level.entities.data, defaultMap, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->level.ground.data, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->level.controllers.data, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
        memset(iw->entityMap, 0, sizeof(Entity*) * MAP_WIDTH * MAP_HEIGHT); 
        iw->level.propertiesScript[0] = '\0';
        
        for(int i = 0; i < MAX_SCRIPTS_PER_LEVEL; ++i)
        {
            iw->level.scripts[i].text[0] = '\0';
            iw->level.scripts[i].inUse = false;
        }
        iw->editor.enabled = true;
    }
    
    for(int i = 0; i < MAX_BACKGROUNDS_PER_LEVEL; ++i)
        iw->backgrounds[i].enabled = false;
    
    ScriptState roomProperties = parseScript(iw->level.propertiesScript);
    if(scriptHasValue(&roomProperties, "roomtype"))
    {
        loadRoomFile(iw, scriptReadString(&roomProperties, "roomtype", "notfound"));
    }
    
    musicPlay(assetsGetMusic(&iw->game, "forestMusic"), 0.7, &iw->game);
    
    destroyAllEntities(iw);
    
    for(int t = 0; t < iw->level.entities.height; ++t)
        for(int i = 0; i < iw->level.entities.width; ++i)
        {
            int index = i + (t * iw->level.entities.width);
            int typeIndex = iw->level.entities.data[index] - 1;
            
            Entity* e = createEntityFromTypeIndex(iw, typeIndex, i * GRID_SIZE, t * GRID_SIZE);
            if(e)
            {
                typeIndex = iw->level.controllers.data[index] - 1;
                entitySetControllerFromTypeIndex(e, typeIndex, iw);
            }
        }
    
    for(int t = 0; t < iw->level.entities.height; ++t)
        for(int i = 0; i < iw->level.entities.width; ++i)
        {
            Entity* e = iw->entityMap[i][t];
            if(e && e->controller->type == ControllerType_chain)
                resolveChain(e, iw);
        }
}

void saveMap(Iwbtg* iw, char* file)
{
    FILE* f = fopen(file, "wb");
    fwrite(iw->level.entities.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
    fwrite(iw->level.controllers.data, sizeof(int) * MAP_WIDTH * MAP_HEIGHT, 1, f);
    
    int scriptCount = 0;
    for(int i = 0; i < MAX_SCRIPTS_PER_LEVEL; ++i)
        if(iw->level.scripts[i].inUse)
            scriptCount++;
    fwrite(&scriptCount, sizeof(int), 1, f);
    for(int i = 0; i < MAX_SCRIPTS_PER_LEVEL; ++i)
        if(iw->level.scripts[i].inUse)
        {
            // TODO: Write only the characters that are actually used.
            fwrite(&iw->level.scripts[i], sizeof(Script), 1, f);
        }
    fwrite(&iw->level.propertiesScript[0], MAX_LEVEL_PROPERTIES_SCRIPT_LENGTH, 1, f);
    
    fclose(f);
}

void loadGame(Iwbtg* iw)
{
    Player* p = &iw->player;
    
    char saveFileName[128];
    snprintf(saveFileName, 128, "save%d.sav", iw->activeSaveSlot);
    
    iw->saveState.room.x = -1;
    iw->saveState.room.y = 0;
    
    bool newGame = true;
    FILE* f;
    if((f = fopen(saveFileName, "r")))
    {
        newGame = false;
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
    
    if(!newGame)
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
    textInputInit(&iw->textInput);
    
    fontInit(&iw->game.font, assetsGetTexture(&iw->game, "font"), 24, 32, "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890,.!?\"'/\\<>()=:-");
    fontSetAllLetterWidth(&iw->game.font, 20);
    
    fontInit(&iw->fontSmall, assetsGetTexture(&iw->game, "fontSmall"), 12, 16,
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890,.!?\"'/\\<>()=:-");
    fontSetAllLetterWidth(&iw->fontSmall, 8);
    
    editorInit(iw);
    
    iw->blockTexture = assetsGetTexture(&iw->game, "block");
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
    
    iw->blockTexture = assetsGetTexture(&iw->game, "stage1BlockTiles");
}

void iwbtgLoad(Iwbtg* iw)
{
    Game* g = &iw->game;
    assetsLoadTexture(g, "assets/kid.png", "kid"); //kid_silhouette
    assetsLoadTexture(g, "assets/block.png", "block");
    assetsLoadTexture(g, "assets/objects.png", "objects");
    assetsLoadTexture(g, "assets/spike.png", "spike");
    assetsLoadTexture(g, "assets/save.png", "save");
    assetsLoadTexture(g, "assets/player_death_particle.png", "playerDeathParticle");
    assetsLoadTexture(g, "assets/game_over.png", "gameOver");
    assetsLoadTexture(g, "assets/warp.png", "warp");
    assetsLoadTexture(g, "assets/player_bullet.png", "playerBullet");
    assetsLoadTexture(g, "assets/font.png", "font");
    assetsLoadTexture(g, "assets/title.png", "title");
    assetsLoadTexture(g, "assets/fruit.png", "fruit");
    assetsLoadTexture(g, "assets/controllers.png", "controllers");
    assetsLoadTexture(g, "assets/moving_platform.png", "movingPlatform");
    assetsLoadTexture(g, "assets/font_small.png", "fontSmall");
    assetsLoadTexture(g, "assets/gl_test.png", "glTest");
    assetsLoadTexture(g, "assets/boss1.png", "boss");
    assetsLoadTexture(g, "assets/boss_health_bar.png", "bossHealthBar");
    assetsLoadTexture(g, "assets/block_tiles.png", "stage1BlockTiles");
    assetsLoadTexture(g, "assets/stage1_back1.png", "stage1Back1");
    assetsLoadTexture(g, "assets/stage1_back2.png", "stage1Back2");
    assetsLoadTexture(g, "assets/stage1_back3.png", "stage1Back3");
    assetsLoadTexture(g, "assets/block_tiles2.png", "stage2BlockTiles");
    assetsLoadTexture(g, "assets/stage2_back1.png", "stage2Back1");
    assetsLoadTexture(g, "assets/stage2_back2.png", "stage2Back2");
    assetsLoadTexture(g, "assets/stage2_back3.png", "stage2Back3");
    assetsLoadTexture(g, "assets/mountains.png", "mountains");
    assetsLoadTexture(g, "assets/night_sky.png", "nightSky");
    assetsLoadTexture(g, "assets/tower.png", "tower");
    assetsLoadTexture(g, "assets/vine.png", "vine");
    
    assetsLoadSound(g, "assets/jump.wav", "jump");
    assetsLoadSound(g, "assets/double_jump.wav", "doubleJump");
    assetsLoadSound(g, "assets/shoot.wav", "shoot");
    assetsLoadSound(g, "assets/death.wav", "death");
    assetsLoadSound(g, "assets/save.wav", "saved");
    assetsLoadSound(g, "assets/trap.wav", "trap");
    assetsLoadSound(g, "assets/boss_hit.wav", "bossHit");
    assetsLoadSound(g, "assets/boss_shoot.wav", "bossShoot");
    
    assetsLoadMusic(g, "assets/forest_music.ogg", "forestMusic");
    assetsLoadMusic(g, "assets/menu_music.ogg", "menuMusic");
    assetsLoadMusic(g, "assets/apple_boss.ogg", "appleBossMusic");
    assetsLoadMusic(g, "assets/boss_defeated.ogg", "bossDefeatedMusic");
    assetsLoadMusic(g, "assets/game_over.ogg", "gameOverMusic");
}

void iwbtgUpdate(Iwbtg* iw)
{
    Game* g = &iw->game;
    float dt = (float)1 / 50;
    iw->time += dt;
    
    iw->frameCount++;
    
    textInputUpdate(&iw->textInput, iw);
    
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
        
        if(checkKeyPressed(g, KEY_MENU) && !iw->editor.enabled)
        {
            iw->state = GameState_menu;
            iw->activeMenu = &iw->mainMenu;
            musicStop(&iw->game);
            musicPlayOnce(assetsGetMusic(&iw->game, "menuMusic"), 1.0, &iw->game);
        }
        
        if(!iw->editor.enabled)
            playerUpdate(&iw->player, iw);
        
        editorUpdate(iw);
        
        iw->entityDrawCount = 0;
        if(!iw->editor.enabled)
        {
            for(int i = 0; i < MAX_ENTITIES; ++i)
            {
                Entity* e = &iw->entities[i];
                if(e->active)
                {
                    iw->entityDrawOrder[iw->entityDrawCount++] = e;
                    entityUpdate(e, iw, dt);
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

void iwbtgDraw(Iwbtg* iw)
{
    Game* g = &iw->game;
    renderBegin(g);
    
    for(int i = 0; i < MAX_BACKGROUNDS_PER_LEVEL; ++i)
    {
        Background* b = &iw->backgrounds[i];
        if(b->enabled)
        {
            int offsetX = (int)(iw->time * b->speed.x) % b->texture->size.x;
            int offsetY = (int)(iw->time * b->speed.y) % b->texture->size.y;
            textureDraw(g, b->texture, 
                        b->position.x + offsetX, 
                        b->position.y + offsetY);
            // TODO: Remove this hack and implement looping properly
            if(b->speed.x > 0)
                textureDraw(g, b->texture, 
                            b->position.x + offsetX - b->texture->size.x, 
                            b->position.y + offsetY);
            if(b->speed.y > 0)
                textureDraw(g, b->texture, 
                            b->position.x + offsetX, 
                            b->position.y + offsetY - b->texture->size.x);
                            
            if(b->speed.x > 0 && b->speed.y > 0)
                textureDraw(g, b->texture, 
                            b->position.x + offsetX - b->texture->size.x, 
                            b->position.y + offsetY - b->texture->size.x);
        }
    }
    
    
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
                entityDraw(e, iw);
            }

        Player* p = &iw->player;
        if(!p->dead)
            spriteDraw(g, &iw->player.sprite, iw->player.position.x, iw->player.position.y);
        
        char roomText[128];
        snprintf(roomText, 128, "%03d", 1 - iw->room.y);
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
        else
        {
            if(iw->boss && iw->boss->controller->boss.triggered)
            {
                ControllerBoss* bc = &iw->boss->controller->boss;
                
                textureDrawExt(g, assetsGetTexture(g, "bossHealthBar"), 
                            24, 16, 912, 32,
                            0, 0, 912, 32,
                            0, 0, 1, 1, 0, 1,
                            0, makeColor(1, 1, 1, 1));
                            
                float hp = (float)bc->health / bc->maxHealth;
                            
                textureDrawExt(g, assetsGetTexture(g, "bossHealthBar"), 
                            24, 16, 2 + (908 * hp), 32,
                            0, 32, 2 + (908 * hp), 32,
                            0, 0, 1, 1, 0, 1,
                            0, makeColor(1, 1, 1, 1));
            }
        }
        
        //rectangleDraw(g, iw->debugDrawPosition.x, iw->debugDrawPosition.y, 4, 4, 1, 0, 1, 1);
    }
    
    textInputDraw(&iw->textInput, iw);
    
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
    
    loadRoomFile(iwbtg, "menu");
    
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