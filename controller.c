int bossGetNextActionIndex(int currentAction)
{
    int index = currentAction + 1;
    if(index >= MAX_ACTIONS_PER_BOSS)
        index = 0;
    return index;
}

bool bossIsActionQueueEmpty(ControllerBoss* b)
{
    return (b->actionQueueTail == b->actionQueueHead);
}

bool bossIsActionQueueFull(ControllerBoss* b)
{
    return (bossGetNextActionIndex(b->actionQueueTail) == b->actionQueueHead);
}

BossAction* bossAddAction(Entity* e, BossActionType type)
{
    ControllerBoss* b = &e->controller->boss;
    if(!bossIsActionQueueFull(b))
    {
        BossAction* a = &b->actionQueue[b->actionQueueTail];
        b->actionQueueTail = bossGetNextActionIndex(b->actionQueueTail);
        a->type = type;
        a->active = true;
        switch(type)
        {
            case BossActionType_wait:
                a->wait.time = 0.1;
                break;
                
            case BossActionType_move:
                a->move.time = 2;  
                a->move.destination = v2f(32, 64);
                break;
                
            case BossActionType_projectileBurst:
                a->projectileBurst.count = 8;
                a->projectileBurst.speed = 8;
                a->projectileBurst.projectileEntityType = EntityType_fruit;
                break;
            
            default:
                break;
        }
        return a;
    }
    else
        printf("Error: Boss action queue was full, could not add another action.\n");
    
    return 0;
}

void bossNextAction(ControllerBoss* b)
{
    b->actionQueue[b->actionQueueHead].active = false;
    if(!bossIsActionQueueEmpty(b))
    {
        b->actionQueueHead = bossGetNextActionIndex(b->actionQueueHead);
        b->actionQueue[b->actionQueueHead].initialized = false;
        b->actionTime = 0;
    }
}

void entitySetControllerFromTypeIndex(Entity* e, int typeIndex, Iwbtg* iw)
{
    e->controller = &e->controllerData;
    
    int x = e->position.x / GRID_SIZE;
    int y = e->position.y / GRID_SIZE;
    
    if(typeIndex == 0) // trap
    {
        e->controller->type = ControllerType_trap;
        e->controller->trap.direction = PI * 0.5f;
        e->controller->trap.activationDistance = 400;
        e->controller->trap.activationWidth = 32;
        e->controller->trap.speed = 16;
        e->controller->trap.activated = 0;
        
        Script* script = levelGetScriptAtPosition(&iw->level, x, y);
        if(script)
        {
            ScriptState ss = parseScript(script->text);
            ControllerTrap* trap = &e->controller->trap;
            trap->direction = scriptReadNumber(&ss, "direction", 90) * (PI / 180);
            trap->speed = scriptReadNumber(&ss, "speed", trap->speed);
            trap->activationDistance = scriptReadNumber(&ss, "activationDistance", trap->activationDistance);
            trap->activationWidth = scriptReadNumber(&ss, "activationWidth", trap->activationWidth);
        }
    }
    else if(typeIndex == 1) // In out
    {
        e->controller->type = ControllerType_inOut;
        e->controller->inOut.direction = 90;
        e->controller->inOut.speed = 1;
        if(typeIndex == 13)
            e->controller->inOut.speed = 2;
        e->controller->inOut.distance = 32;
        e->controller->inOut.timer = 0;
        e->controller->inOut.basePosition = e->position;
        
        Script* script = levelGetScriptAtPosition(&iw->level, x, y);
        if(script)
        {
            ScriptState ss = parseScript(script->text);
            ControllerInOut* inOut = &e->controller->inOut;
            inOut->direction = scriptReadNumber(&ss, "direction", inOut->direction)  * (PI / 180);
            inOut->speed = scriptReadNumber(&ss, "speed", inOut->speed);
            inOut->distance = scriptReadNumber(&ss, "distance", inOut->distance);
        }
    }
    else if(typeIndex == 2) // Bounce
    {
        e->controller->type = ControllerType_bounce;
        
        Script* script = levelGetScriptAtPosition(&iw->level, x, y);
        if(script)
        {
            ScriptState ss = parseScript(script->text);
            e->velocity.x = scriptReadNumber(&ss, "hspeed", e->velocity.x);
            e->velocity.y = scriptReadNumber(&ss, "vspeed", e->velocity.y);
        }
    }
    else if(typeIndex == 63) // Chaining controllers
    {
        e->controller->type = ControllerType_chain;
    }
}

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
        
        case ControllerType_bounce: {
            Rectanglef hitbox = { 0, 0, e->sprite.size.x, e->sprite.size.y };
            
            if(rectangleIsCollidingWithGround(&hitbox, iw, e->position.x, e->position.y))
                e->velocity = vector2fMultiply(e->velocity, -1);
        } break;
        
        case ControllerType_boss: {
            
            ControllerBoss* b = &c->boss;
            
            b->actionTime += dt;
            if(b->actionQueue[b->actionQueueHead].active)
            {
                BossAction* a = &b->actionQueue[b->actionQueueHead];
                
                switch(a->type)
                {
                    case BossActionType_wait:
                        if(b->actionTime > a->wait.time)
                            bossNextAction(b);
                        break;
                        
                    case BossActionType_move:
                        if(!a->initialized)
                        {
                            a->move.start = e->position;
                            a->initialized = true;
                        }
                        
                        e->position = vector2fLerp(a->move.start, a->move.destination, inOutEase(clamp(b->actionTime / a->move.time, 0, 1)));
                        
                        if(b->actionTime >= a->move.time)
                            bossNextAction(b);
                        break;
                        
                    case BossActionType_projectileBurst:
                        
                        for(int i = 0; i < a->projectileBurst.count; ++i)
                        {
                            Entity* p = createEntity(iw, a->projectileBurst.projectileEntityType, 
                                e->position.x + (e->sprite.size.x / 2), e->position.y + (e->sprite.size.y / 2));
                            p->position.x -= (p->sprite.size.x / 2);
                            p->position.y -= (p->sprite.size.y / 2);
                            p->velocity = speedDirectionToVector2f(a->projectileBurst.speed, (360 / a->projectileBurst.count) * i);
                        }
                        
                        bossNextAction(b);
                        break;
                        
                    default:
                        break;
                }
            }
            else
                bossNextAction(b);
            
            if(b->health <= 0)
            {
                if(iw->level.boss == e)
                    iw->level.boss = 0;
                createEntity(iw, EntityType_warp, 480 - 64, 270 - 64);
                destroyEntity(e);
            }
            
        } break;
        
        default:
        
            break;
    }
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