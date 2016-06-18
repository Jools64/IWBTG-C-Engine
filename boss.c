void initializeBoss1(Entity* e, Iwbtg* iw)
{
    e->position.x = e->position.x - 64 + 16;
    e->position.y = - 128;//- 64 + 16;
    spriteInit(&e->sprite, assetsGetTexture(&iw->game, "boss"), 128, 128);
}

void initializeBoss2(Entity* e, Iwbtg* iw)
{
    e->position.x = e->position.x - 128 + 16;
    e->position.y = - 128;
    spriteInit(&e->sprite, assetsGetTexture(&iw->game, "boss2"), 256, 256);
}

// TODO: Remove this temporary code
void addFirstBossActions(Entity* e, Iwbtg* iw)
{
    BossAction* a;
    
    // Move on screen / Reset position on loop
    a = bossAddAction(e, BossActionType_playMusic);
    a->playMusic.music = assetsGetMusic(&iw->game, "appleBossMusic");
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(480 - 64, 128);
    a->move.time = 1;
    
    // Shoot aimed shots at the player
    for(int i = 0; i < 8; ++i)
    {
        a = bossAddAction(e, BossActionType_projectileAimed);
        a = bossAddAction(e, BossActionType_wait);
        a->wait.time = 0.3;
    }
    
    
    // Random spray
    a = bossAddAction(e, BossActionType_projectileRandomSpray);
    a->projectileRandomSpray.time = 4;
    a = bossAddAction(e, BossActionType_wait);
    a->wait.time = 1;
    
    // Burst bullets and stuff
    a = bossAddAction(e, BossActionType_wait);
    a->wait.time = 0.2;
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(128, 128);
    a->move.time = 1;
    a = bossAddAction(e, BossActionType_projectileBurst);
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(960 - 128 - 128, 128);
    a = bossAddAction(e, BossActionType_projectileBurst);
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(480 - 64, 128);
    a->move.time = 1;
    a = bossAddAction(e, BossActionType_projectileBurst);
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(480 - 64, 320);
    a->move.time = 1;
    a = bossAddAction(e, BossActionType_projectileBurst);
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(480 - 64, 128);
    a = bossAddAction(e, BossActionType_projectileBurst);
    a = bossAddAction(e, BossActionType_wait);
    a->wait.time = 2;
    
    // Fly around the edges of the screen
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(480 - 64, 64);
    a->move.time = 0.2;
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(960 - 128 - 64, 64);
    a->move.time = 1.0;
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(960 - 128  - 64, 540 - 128);
    a->move.time = 1.0;
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(64, 540 - 128);
    a->move.time = 1.0;
    a = bossAddAction(e, BossActionType_move);
    a->move.destination = v2f(64, 0);
    a->move.time = 1.0;
}

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
    return (bossGetNextActionIndex(b->actionQueueTail) == b->actionQueueHead 
         || b->actionQueue[bossGetNextActionIndex(b->actionQueueTail)].active);
}

bool bossIsActionBlocking(ControllerBoss* b)
{
    for(int i = 0; i < MAX_ACTIONS_PER_BOSS; ++i)
        if(b->activeActions[i] && !b->activeActions[i]->parallel)
            return true;
    return false;
}

void bossAddActiveAction(ControllerBoss* b, BossAction* action)
{
    for(int i = 0; i < MAX_ACTIONS_PER_BOSS; ++i)
        if(!b->activeActions[i])
        {
            b->activeActions[i] = action;
            return;
        }
}

void bossRemoveActiveAction(ControllerBoss* b, BossAction* action)
{
    for(int i = 0; i < MAX_ACTIONS_PER_BOSS; ++i)
        if(b->activeActions[i] == action)
        {
            b->activeActions[i] = 0;
            return;
        }
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
        a->time = 0;
        switch(type)
        {
            case BossActionType_wait:
                a->wait.time = 0.1;
                a->parallel = false;
                break;
                
            case BossActionType_move:
                a->move.time = 2;  
                a->move.destination = v2f(32, 64);
                a->parallel = false;
                break;
                
            case BossActionType_projectileBurst:
                a->projectileBurst.count = 8;
                a->projectileBurst.speed = 8;
                a->projectileBurst.repeat = 4;
                a->projectileBurst.interval = 0.3;
                a->projectileBurst.direction = 0;
                a->projectileBurst.rotation = 20;
                a->projectileBurst.projectileEntityType = EntityType_fruit;
                a->parallel = true;
                break;
                
            case BossActionType_projectileRandomSpray:
                a->projectileRandomSpray.projectileEntityType = EntityType_fruit;
                a->projectileRandomSpray.time = 20;
                a->projectileRandomSpray.speed = 6;
                a->projectileRandomSpray.previousDirection = 0;
                a->parallel = false;
                break;
                
            case BossActionType_projectileAimed:
                a->projectileAimed.projectileEntityType = EntityType_fruit;
                a->projectileAimed.speed = 8;
                a->parallel = false;
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
        bossAddActiveAction(b, &b->actionQueue[b->actionQueueHead]);
    }
}

void bossControllerUpdate(Controller* c, Entity* e, Iwbtg* iw, float dt)
{
    ControllerBoss* b = &c->boss;
            
    if(b->triggered)
    {
        if(!b->initialized)
        {
            b->initialized = true;
            
            // Take the first action in the queue
            if(!bossIsActionQueueEmpty(b))
            {
                b->actionQueue[0].initialized = false;
                bossAddActiveAction(b, &b->actionQueue[0]);
            }
        }

        for(int i = 0; i < MAX_ACTIONS_PER_BOSS; ++i)
        {
            BossAction* a = b->activeActions[i];
            if(a == 0)
                continue;
            
            a->time += dt;
            
            switch(a->type)
            {
                case BossActionType_wait:
                    if(a->time > a->wait.time)
                        bossRemoveActiveAction(b, a);
                    break;
                    
                case BossActionType_move:
                    if(!a->initialized)
                    {
                        a->move.start = e->position;
                        a->initialized = true;
                    }
                    
                    e->position = vector2fLerp(a->move.start, a->move.destination, inOutEase(clamp(a->time / a->move.time, 0, 1)));
                    
                    if(a->time >= a->move.time)
                        bossRemoveActiveAction(b, a);
                    break;
                    
                case BossActionType_projectileBurst: {
                    
                    BossActionProjectileBurst* pb = &a->projectileBurst;
                    
                    if(!a->initialized)
                        pb->shotTimer = 0;
                    
                    pb->shotTimer += dt;
                    
                    // TODO: do something a little smarter here so that
                    //       if two shots occur on the same frame, their positions
                    //       are correctly seperated from each other.
                    if(pb->shotTimer > pb->interval)
                    {
                        soundPlay(assetsGetSound(&iw->game, "bossShoot"), 0.2);
                        while(pb->shotTimer > pb->interval)
                        {
                            for(int i = 0; i < pb->count; ++i)
                            {
                                Entity* p = createEntity(iw, pb->projectileEntityType, 
                                    e->position.x + (e->sprite.size.x / 2), e->position.y + (e->sprite.size.y / 2));
                                p->position.x -= (p->sprite.size.x / 2);
                                p->position.y -= (p->sprite.size.y / 2);
                                p->velocity = speedDirectionToVector2f(pb->speed, pb->direction + ((360 / pb->count) * i));
                                
                            }
                            
                            pb->direction += pb->rotation;
                            pb->shotTimer -= pb->interval;
                        }
                    }
                    
                    if(a->time > pb->interval * pb->repeat)
                        bossRemoveActiveAction(b, a);
                } break;
                    
                case BossActionType_playMusic:
                    musicPlay(a->playMusic.music, 0.7, &iw->game);
                    bossRemoveActiveAction(b, a);
                    break;
                    
                case BossActionType_projectileRandomSpray:
                    if(a->time > a->projectileRandomSpray.time)
                        bossRemoveActiveAction(b, a);
                    
                    BossActionProjectileRandomSpray* pb = &a->projectileRandomSpray;
                    
                    Entity* p = createEntity(iw, pb->projectileEntityType, 
                                    e->position.x + (e->sprite.size.x / 2), e->position.y + (e->sprite.size.y / 2));
                    p->position.x -= (p->sprite.size.x / 2);
                    p->position.y -= (p->sprite.size.y / 2);
                    pb->previousDirection += (12 * 7);
                    //pb->previousDirection = (randomfBetween(0, 360));
                    p->velocity = speedDirectionToVector2f(pb->speed, pb->previousDirection + randomfBetween(-12, 12));
                    
                    pb->soundCounter++;
                    if(pb->soundCounter > 1)
                    {
                        pb->soundCounter = 0;
                        soundPlay(assetsGetSound(&iw->game, "bossShoot"), 0.1);
                    }
                    break;
                    
                case BossActionType_projectileAimed: {
                    BossActionProjectileAimed* pa = &a->projectileAimed;
                    Entity* p = createEntity(iw, pa->projectileEntityType, 
                                e->position.x + (e->sprite.size.x / 2), e->position.y + (e->sprite.size.y / 2));
                    p->position.x -= (p->sprite.size.x / 2);
                    p->position.y -= (p->sprite.size.y / 2);
                    
                    float direction = vector2fDirection(
                        vector2fAdd(e->position, v2f(e->sprite.size.x / 2, e->sprite.size.y / 2)),
                        vector2fAdd(iw->player.position, v2f(16, 16)))
                            * (180 / PI);
                    
                    p->velocity = speedDirectionToVector2f(pa->speed, direction);
                    
                    soundPlay(assetsGetSound(&iw->game, "bossShoot"), 0.1);
                    
                    bossRemoveActiveAction(b, a);
                } break;
                   
                default:
                    bossRemoveActiveAction(b, a);
                    printf("Warning: Tried to perform boss action that has not been implemented!\n");
                    break;
            }
            
            if(!a->initialized)
                a->initialized = true;
            
        }
        
        // TODO: Make this into a loop so multiple actions can start per frame.
        if(!bossIsActionBlocking(b) && iw->state != GameState_gameOver)
            bossNextAction(b);
        
        if(b->health <= 0)
        {
            if(iw->boss == e)
                iw->boss = 0;
            createEntity(iw, EntityType_warp, 480 - 64, 540 - 256);
            musicPlayOnce(assetsGetMusic(&iw->game, "bossDefeatedMusic"), 0.7, &iw->game);
            destroyEntity(e);
        }
    }
}