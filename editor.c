void saveMap(Iwbtg* iw, char* file);
void loadMap(Iwbtg* iw, char* file);
char* getCurrentMapName(Iwbtg* iw, char* string, int stringLength);

void editorInit(Iwbtg* iw)
{
    Editor* e = &iw->editor;
    
    gridInit(&iw->level.entities, MAP_WIDTH, MAP_HEIGHT, &iw->game.globalMemory);
    gridInit(&iw->level.ground, MAP_WIDTH, MAP_HEIGHT, &iw->game.globalMemory);
    gridInit(&iw->level.controllers, MAP_WIDTH, MAP_HEIGHT, &iw->game.globalMemory);
    
    e->objectsTexture = assetsGetTexture(&iw->game, "objects");
    e->controllersTexture = assetsGetTexture(&iw->game, "controllers");
    
    spriteInit(&e->objectSprite, e->objectsTexture, 32, 32);
    spriteInit(&e->controllerSprite, e->controllersTexture, 32, 32);
    
    iw->editor.selectedObject = 1;
    iw->state = GameState_menu;
    
    e->mode = EditorMode_entities;
}

void editorUpdate(Iwbtg* iw)
{
    Game* g = &iw->game;
    Editor* e = &iw->editor;
    
    if(iw->editor.enabled)
    {
        int mx = iw->game.input.mousePosition.x / 32;
        int my = iw->game.input.mousePosition.y / 32;
        
        if(e->mode == EditorMode_entities || e->mode == EditorMode_controllers)
        {
            Grid* grid = &iw->level.entities;
            if(e->mode == EditorMode_controllers)
                grid = &iw->level.controllers;
                
            if(checkMouseButton(g, SDL_BUTTON_LEFT))
            {
                
                if(checkKey(g, KEY_TILE_PICKER))
                    iw->editor.selectedObject = mx + (my * (e->objectsTexture->size.x / 32)) + 1;
                else
                    gridSet(grid, mx, my, e->selectedObject);
            }
            else if(checkMouseButton(g, SDL_BUTTON_RIGHT))
                gridSet(grid, mx, my, 0);
        }
        else if(e->mode == EditorMode_scripts)
        {
            if(checkMouseButton(g, SDL_BUTTON_LEFT))
            {
                iw->textInput.active = true;
            }
        }
    }
            
    if(checkKeyPressed(&iw->game, KEY_LEFT))
    {
        e->mode--;
        if(e->mode < 0)
            e->mode = EditorMode_count-1;
    }
    if(checkKeyPressed(&iw->game, KEY_RIGHT))
    {
        e->mode++;
        if(e->mode > EditorMode_count-1)
            e->mode = 0;
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
}

void editorDraw(Iwbtg* iw)
{
    Game* g = &iw->game;
    Editor* e = &iw->editor;
    
    if(e->enabled)
    {
        
        for(int i = 0; i < iw->level.entities.width; ++i)
            for(int t = 0; t < iw->level.entities.height; ++t)
            {
                int index = i + (t * iw->level.entities.width);
                
                Sprite* objectSprite = &iw->editor.objectSprite;
                objectSprite->frame = iw->level.entities.data[index] - 1;
                if(objectSprite->frame >= 0)
                    spriteDraw(g, objectSprite, i * 32, t * 32);
                
                Sprite* controllerSprite = &iw->editor.controllerSprite;
                controllerSprite->frame = iw->level.controllers.data[index] - 1;
                if(controllerSprite->frame >= 0)
                    spriteDraw(g, controllerSprite, i * 32, t * 32);
            }
        
        if(e->mode == EditorMode_entities || e->mode == EditorMode_controllers)
        {
            Sprite* selectedSprite = &e->objectSprite;
            if(e->mode == EditorMode_controllers)
                selectedSprite = &e->controllerSprite;
            
            selectedSprite->frame = e->selectedObject-1;
            spriteDraw(g, selectedSprite, 16, 16);
            
            if(checkKey(g, KEY_TILE_PICKER))
            {
                if(e->mode == EditorMode_entities)
                    textureDraw(g, e->objectsTexture, 0, 0);
                else if(e->mode == EditorMode_controllers)
                    textureDraw(g, e->controllersTexture, 0, 0);
            }
        }
        
        if(e->mode == EditorMode_entities)
            drawText(&iw->game, 0, "ENTITIES", 8, 540 - 32 - 8);
        else if(e->mode == EditorMode_controllers)
            drawText(&iw->game, 0, "CONTROLLERS", 8, 540 - 32 - 8);
        else if(e->mode == EditorMode_scripts)
            drawText(&iw->game, 0, "SCRIPTS", 8, 540 - 32 - 8);
    }
}