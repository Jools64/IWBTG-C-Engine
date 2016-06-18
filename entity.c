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
    e->controller = &e->controllerData;
    e->controller->hasChains = false;
    e->controller->type = ControllerType_none;
    
    // Important: All entities must have a sprite. (Or I need to implement behavior to handle this)
    switch(type)
    {
        case EntityType_block:
            spriteInit(&e->sprite, iw->blockTexture, 16, 16);
            e->depth = -1;
			break;
            
        case EntityType_spike:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "spike"), 32, 32);
            e->sprite.origin.x = 16; //e->sprite.size.x / 2;
            e->sprite.origin.y = 16; //e->sprite.size.y / 2;
            break;
            
        case EntityType_save:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "save"), 32, 32);
            e->animationTimer = 0.05f;
            break;
            
        case EntityType_warp:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "warp"), 128, 128);
            e->depth = -2;
            break;
            
        case EntityType_playerBullet: {
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "playerBullet"), 4, 4);
            int frames[] = { 0, 1 };
            spriteAddAnimation(&e->sprite, Animations_default, &frames[0], 2, 12);
            spritePlayAnimation(&e->sprite, Animations_default);
            e->depth = -3;
        } break;
        
        case EntityType_fruit:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "fruit"), 32, 32);
            int frames[] = { 0, 1 };
            spriteAddAnimation(&e->sprite, Animations_default, &frames[0], 2, 6);
            spritePlayAnimation(&e->sprite, Animations_default);
            e->depth = -3;
            break;
            
        case EntityType_movingPlatform:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "movingPlatform"), 32, 16);
            e->depth = 0;
            break;
            
        case EntityType_boss:
            e->position.x = x;
            e->position.y = y;
            e->controller->type = ControllerType_boss;
            e->controller->boss.health = e->controller->boss.maxHealth = 30;
            e->controller->boss.actionQueueHead = e->controller->boss.actionQueueTail = 0;
            e->controller->boss.initialized = false;
            e->controller->boss.triggered = false;
            for(int i = 0; i < MAX_ACTIONS_PER_BOSS; ++i)
            {
                e->controller->boss.activeActions[i] = 0;
                e->controller->boss.actionQueue[i].active = 0;
            }
            e->depth = -4;
            
            if(iw->saveState.room.y == -19)
                initializeBoss1(e, iw);
            if(iw->saveState.room.y == -39)
                initializeBoss2(e, iw);
            break;
            
        case EntityType_vine:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "vine"), 32, 32);
            e->depth = -2;
            break;
            
        case EntityType_jumpRefresher:
            spriteInit(&e->sprite, assetsGetTexture(&iw->game, "jumpRefresher"), 32, 32);
            e->depth = -2;
            break;
            
        default:
            break;
    }
    
    return e;
}

Entity* createEntityFromTypeIndex(Iwbtg* iw, int typeIndex, int x, int y)
{
    Entity* e = 0;
    int i = x / GRID_SIZE;
    int t = y / GRID_SIZE;
    
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
    else if(typeIndex == 8)
    {
        e = createEntity(iw, EntityType_boss, x, y);
        iw->boss = e;
    }
    else if(typeIndex == 9)
        e = createEntity(iw, EntityType_jumpRefresher, x, y);
    iw->entityMap[i][t] = e;
    
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

void entityUpdate(Entity* e, Iwbtg* iw, float dt)
{
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
                {
                    soundPlay(assetsGetSound(&iw->game, "saved"), 1);
                    saveGame(iw, true);
                    
                    if(iw->boss)
                    {
                        destroyEntity(e);
                        iw->boss->controller->boss.triggered = true;
                    }
                }
                e->sprite.frame = 1;
                e->animationTimer = 0.5;
            }
            if(e->animationTimer > 0)
            {
                e->animationTimer -= dt;
                e->sprite.scale.y = e->sprite.scale.x = 1 + inOutEase(e->animationTimer);
                if(e->animationTimer <= 0)
                {
                    e->sprite.scale.x = e->sprite.scale.y = 1;
                    e->sprite.frame = 0;
                }
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
            
            // Lazy
            Rectanglef hitbox = { 0, 0, 4, 4 };
            Rectanglef hitbox2 = { e->position.x, e->position.y, 4, 4 };
            
            Entity* other;
            if(rectangleIsCollidingWithGround(&hitbox, iw, e->position.x, e->position.y))
                destroyEntity(e);
            else if((other = rectangleCheckCollision(&hitbox2, iw, EntityType_boss)))
            {
                other->controller->boss.health--;
                destroyEntity(e);
                soundPlay(assetsGetSound(&iw->game, "bossHit"), 0.7);
            }
            
            break;
            
        case EntityType_fruit:
            if(e->position.x > iw->game.size.x
            || e->position.y > iw->game.size.y
            || e->position.x < -e->sprite.size.x
            || e->position.y < -e->sprite.size.y)
                destroyEntity(e);
            break;
            
        case EntityType_movingPlatform: {
            
            if(entityCheckPlayerCollision(e, &iw->player))
            {
                Vector2f pos = iw->player.position;
                if(iw->player.position.y < e->position.y - 24
                   && iw->player.velocity.y > 0)
                {
                    iw->player.position.y = e->position.y - 32;
                    if(playerIsCollidingWithGround(&iw->player, iw, 0, 0))
                        iw->player.position = pos;
                }
            }
            
            if(entityCheckPlayerCollisionAtOffset(e, &iw->player, 0, -1)
               && !entityCheckPlayerCollisionAtOffset(e, &iw->player, 0, 0))
            {
                iw->player.position = vector2fAdd(iw->player.position, e->velocity);
                if(playerIsCollidingWithGround(&iw->player, iw, 0, e->velocity.y))
                    iw->player.position = vector2fSubtract(iw->player.position, e->velocity);
            }
        } break;
        
        case EntityType_boss: {
            if(bossIsActionQueueEmpty(&e->controller->boss))
                addFirstBossActions(e, iw);
        } break;
        
        case EntityType_jumpRefresher:
            
            e->position.y -= sin(iw->time * 8) * 0.2;
            
            if(entityCheckPlayerCollisionAtOffset(e, &iw->player, 0, 0))
            {
                iw->player.doubleJumpAvailible = true;
                destroyEntity(e);
            }
            
            break;
        
        default:
            break;
    }
    
    e->velocity = vector2fAdd(e->velocity, e->acceleration);
    e->position = vector2fAdd(e->position, e->velocity);
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

void entityDraw(Entity* e, Iwbtg* iw)
{
    Game* g = &iw->game;
    
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
              
            case EntityType_boss: {
                if(e->controller->boss.triggered)
                    spriteDraw(g, &e->sprite, e->position.x, e->position.y);
            } break;
              
            default:
                spriteDraw(g, &e->sprite, e->position.x, e->position.y);
                break;
        }
    }
}