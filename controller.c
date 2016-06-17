#include "boss.c"

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
        e->controller->inOut.speed = 0.5;
        if(typeIndex == 13)
            e->controller->inOut.speed = 2;
        e->controller->inOut.distance = 32;
        e->controller->inOut.timer = 0;
        e->controller->inOut.interval = 0;
        e->controller->inOut.basePosition = e->position;
        e->controller->inOut.ease = true;
        
        Script* script = levelGetScriptAtPosition(&iw->level, x, y);
        if(script)
        {
            ScriptState ss = parseScript(script->text);
            ControllerInOut* inOut = &e->controller->inOut;
            inOut->direction = scriptReadNumber(&ss, "direction", inOut->direction)  * (PI / 180);
            inOut->speed = scriptReadNumber(&ss, "speed", inOut->speed);
            inOut->distance = scriptReadNumber(&ss, "distance", inOut->distance);
            inOut->interval = scriptReadNumber(&ss, "interval", inOut->interval);
            inOut->timer = scriptReadNumber(&ss, "offset", inOut->timer);
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
    else if(typeIndex == 3) // Boo circle
    {
        e->controller->type = ControllerType_booCircle;
        
        Script* script = levelGetScriptAtPosition(&iw->level, x, y);
        ControllerBooCircle* bc = &e->controller->booCircle;
        
        bc->origin = e->position;
        
        bc->speed = 1;
        bc->distance = 96;
        bc->count = 8;
        
        if(script)
        {
            ScriptState ss = parseScript(script->text);
            bc->speed = scriptReadNumber(&ss, "speed", bc->speed);
            bc->distance = scriptReadNumber(&ss, "distance", bc->distance);
            bc->count = scriptReadNumber(&ss, "count", bc->count);
            bc->timer = scriptReadNumber(&ss, "offset", bc->count);
        }
        
        bc->count = clamp(bc->count, 0, MAX_ENTITIES_PER_BOO_CIRCLE);
        
        bc->entities[0] = e;
        for(int i = 1; i < bc->count; ++i)
            bc->entities[i] = createEntity(iw, e->type, e->position.x, e->position.y);
        
        for(int i = 0; i < bc->count; ++i)
        {
            Entity* e = bc->entities[i];
            e->position = vector2fAdd(bc->origin, 
                speedDirectionToVector2f(bc->distance, ((float)360 / bc->count) * i));
        }
    }
    else if(typeIndex == 4 || typeIndex == 5 || typeIndex == 6) // Vine
    {
        Entity* vine = createEntity(iw, EntityType_vine, e->position.x, e->position.y);
        if(typeIndex == 5)
            vine->sprite.frame = 1;
        if(typeIndex == 6)
            vine->sprite.frame = 2;
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
            if(io->timer > 1 + (io->interval / io->speed))
                io->timer -= 1 + (io->interval / io->speed);
            float phase = fabs(1 - (clamp(io->timer, 0, 1) * 2));//fabs(sinf(io->timer));
            if(io->ease)
                phase = inOutEase(phase);
            
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
            
            bossControllerUpdate(c, e, iw, dt);
            
        } break;
        
        case ControllerType_booCircle: {
            
            ControllerBooCircle* bc = &e->controller->booCircle;

            bc->timer += bc->speed * dt * 0.1;
            
            for(int i = 0; i < bc->count; ++i)
            {
                Entity* e = bc->entities[i];
                e->position = vector2fAdd(bc->origin, 
                    speedDirectionToVector2f(bc->distance, (((float)360 / bc->count) * i) + (bc->timer * 360)));
            }
            
        } break;
        
        default:
        
            break;
    }
}

void addChainLink(Level* l, unsigned char chainLinks[MAP_WIDTH][MAP_HEIGHT], int x, int y, Controller** joinedController, Iwbtg* iw)
{
    if(x > 0 && y > 0 && x < MAP_WIDTH && y < MAP_HEIGHT 
       && chainLinks[x][y] == 0)
    {
        int c = l->controllers.data[x + (y * l->entities.width)] - 1;

        if(c == 63) // chain link
        {
            chainLinks[x][y] = true;
            addChainLink(l, chainLinks, x-1, y, joinedController, iw);
            addChainLink(l, chainLinks, x+1, y, joinedController, iw);
            addChainLink(l, chainLinks, x, y-1, joinedController, iw);
            addChainLink(l, chainLinks, x, y+1, joinedController, iw);
        }
        else if(c != -1 && iw->entityMap[x][y]) // controller is something other than chain link
            *joinedController = iw->entityMap[x][y]->controller;
    }
}

void resolveChain(Entity* e, Iwbtg* iw)
{
    unsigned char chainLinks[MAP_WIDTH][MAP_HEIGHT];
    Controller* joinedController = 0;
    memset(chainLinks, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));
    addChainLink(&iw->level, chainLinks, e->position.x / 32, e->position.y / 32, &joinedController, iw);
    
    if(joinedController != 0)
    {
        joinedController->hasChains = true;
        for(int i = 0; i < MAP_WIDTH; ++i)
            for(int t = 0; t < MAP_HEIGHT; ++t)
            {
                if(chainLinks[i][t])
                {
                    Entity* e = iw->entityMap[i][t];
                    if(e)
                        e->controller = joinedController;
                }
            }
    }
}