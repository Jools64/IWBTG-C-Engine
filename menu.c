void menuInit(Menu* m, float x, float y, Iwbtg* iw)
{
    m->itemCount = 0;
    m->firstItem = m->selected = 0;
    m->position.x = x;
    m->position.y = y;
    m->spacing.x = m->spacing.y = 10;
    m->orientation = MenuOrientation_vertical;
    m->cursorPosition.x = m->cursorPosition.y = m->cursor2Position.x = m->cursor2Position.y = 0;
    m->idCounter = 0;
    int frames[] = { 0, 1 };
    spriteInit(&m->cursor, assetsGetTexture(&iw->game, "fruit"), 32, 32);
    spriteAddAnimation(&m->cursor, Animations_default, &frames[0], 2, 6);
    spritePlayAnimation(&m->cursor, Animations_default);
    m->cursor.visible = false;
}

MenuItem* menuAddItem(Menu* m, MenuItemType type, char* label, Iwbtg* iw)
{
    // Fetch the first non-used menu item
    if(m->itemCount == MAX_ELEMENTS_PER_MENU)
    {
        printf("Error: No more room for menu items!\n");
        return 0;
    }
    MenuItem* e = &m->items[m->itemCount++];
    
    // Setup the menu item
    e->id = m->idCounter++;
    e->size = getTextSize(&iw->game.font, label);
    e->type = type;
    e->next = e->previous = 0;
    e->function = 0;
    e->functionData = 0;
    e->lastDrawPosition.x = e->lastDrawPosition.y = 0;
    if(strlen(label) < MAX_LABEL_LENGTH)
        strcpy(e->label, label);
    else
        printf("Error: Menu item label too long!\n");
    
    // Add the item to the menu
    if(m->firstItem == 0)
    {
        m->firstItem = e;
        m->selected = e;
    }
    else
    {
        MenuItem* i = m->firstItem;
        while(i != 0)
        {
            if(i->next == 0)
            {
                i->next = e;
                e->previous = i;
                break;
            }
            i = i->next;
        }
    }
    
    m->lastItem = e;
    
    return e;
}

void updateMenu(Menu* m, Iwbtg* iw, float delta)
{
    Game* g = &iw->game;
    
    if(m->selected)
    {        
        int keyPrevious = KEY_UP;
        int keyNext = KEY_DOWN;
        if(m->orientation == MenuOrientation_horizontal)
        {
            keyPrevious = KEY_LEFT;
            keyNext = KEY_RIGHT;
        }
        
        if(checkKeyPressed(g, keyNext))
        {
            soundPlay(assetsGetSound(&iw->game, "jump"), 1);
            if(m->selected->next)
                m->selected = m->selected->next;
            else
                m->selected = m->firstItem;
        }
        
        if(checkKeyPressed(g, keyPrevious))
        {
            soundPlay(assetsGetSound(&iw->game, "jump"), 1);
            if(m->selected->previous)
                m->selected = m->selected->previous;
            else
                m->selected = m->lastItem;
        }
        
        if(m->selected->type == MenuItemType_button)
        {
            if(checkKeyPressed(g, KEY_JUMP) || checkKeyPressed(g, KEY_SHOOT))
            {
                soundPlay(assetsGetSound(&iw->game, "doubleJump"), 1);
                if(m->selected && m->selected->function)
                    m->selected->function(m->selected, m->selected->functionData);
            }
        }
    }
    
    Vector2f cursorDestination = {
        m->selected->lastDrawPosition.x - (m->selected->size.x / 2), 
        m->selected->lastDrawPosition.y
    };
    
    Vector2f cursor2Destination = {
        m->selected->lastDrawPosition.x + (m->selected->size.x / 2), 
        m->selected->lastDrawPosition.y
    };
    
    if(m->cursorPosition.x <= 0 && m->cursorPosition.y <= 0)
    {
        m->cursorPosition = cursorDestination;
        m->cursor2Position = cursor2Destination;
        m->cursor.visible = false;
    }
    else
    {
        m->cursor.visible = true;
        m->cursorPosition = vector2fLerp(m->cursorPosition, cursorDestination, 0.2);
        m->cursor2Position = vector2fLerp(m->cursor2Position, cursor2Destination, 0.2);
    }
    spriteUpdate(&m->cursor, delta);
}

void drawMenu(Menu* m, Iwbtg* iw)
{
    // Draw each menu item
    Vector2f offset = m->position;
    for(MenuItem* e = m->firstItem; e != 0; e = e->next)
    {
        Color c = iw->game.font.sprite.color;
        if(m->selected != e)
            iw->game.font.sprite.color = makeColor(0.6, 0.6, 0.7, 1.0);
        e->lastDrawPosition.x = offset.x;
        e->lastDrawPosition.y = offset.y;
        drawTextCentered(&iw->game, 0, e->label, offset.x, offset.y);
        iw->game.font.sprite.color = c;
        
        if(m->orientation == MenuOrientation_vertical)
            offset.y += e->size.y + m->spacing.y;
        else if(m->orientation == MenuOrientation_horizontal)
            offset.x += e->size.x + m->spacing.x;
    }
    
    float ox = m->cursorPosition.x - (m->cursor.size.x/2);
    float oy = m->cursorPosition.y - (m->cursor.size.y/2) + 16;
    spriteDraw(&iw->game, &m->cursor, ox - 24, oy);
    
    ox = m->cursor2Position.x - (m->cursor.size.x/2);
    oy = m->cursor2Position.y - (m->cursor.size.y/2) + 16;
    spriteDraw(&iw->game, &m->cursor, ox + 20, oy);
}