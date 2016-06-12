bool entityCheckPlayerCollision(Entity* e, Player* p)
{
    if(e->active)
        return checkRectangleIntersectSprite(&p->hitBox, &p->position, &e->sprite, &e->position);
    return false;
}

bool entityCheckPlayerCollisionAtOffset(Entity* e, Player* p, int x, int y)
{
    e->position.x += x;
    e->position.y += y;
    
    bool result;
    
    if(e->active)
        result = checkRectangleIntersectSprite(&p->hitBox, &p->position, &e->sprite, &e->position);
    
    e->position.x -= x;
    e->position.y -= y;
    
    return result;
}

Entity* rectangleCheckCollision(Rectanglef* r, Iwbtg* iw, EntityType type)
{
    Vector2f offset = { 0.0f, 0.0f };
    
    for(int i = 0; i < MAX_ENTITIES; ++i)
    {
        Entity* e = &iw->entities[i];
        if(e->active && e->type == type)
        {
            if(checkRectangleIntersectSprite(r, &offset, &e->sprite, &e->position))
                return e;
        }
    }
    
    return 0;
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
    || (playerCheckCollisionAtOffset(p, iw, EntityType_movingPlatform, offsetX, offsetY)
    && !playerCheckCollisionAtOffset(p, iw, EntityType_movingPlatform, offsetX, offsetY-1) && p->velocity.y >= 0);
}