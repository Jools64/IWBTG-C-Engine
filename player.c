void playerInit(Player* p, float x, float y, Iwbtg* iw)
{
    p->position.x = x;
    p->position.y = y;
    
    // Note: The the player hitbox changes depending on facing direction.
    //       x and width values should not be adjusted here.
    p->hitBox.x = 9.0f;
    p->hitBox.y = 11.0f;
    p->hitBox.w = 14.0f; 
    p->hitBox.h = 21.0f;
    
    spriteInit(&p->sprite, assetsGetTexture(&iw->game, "kid"), 32, 32);
    
    int stand[] = {0, 1, 2, 3};
    int run[] = {4, 5, 6, 7};
    int jump[] = {10, 11};
    int fall[] = {12, 13};
    int slide[] = {14, 15};
    spriteAddAnimation(&p->sprite, Animations_stand, &stand[0], 4, 8);
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
		
        Entity* bossCollision = playerCheckCollision(p, iw, EntityType_boss);
        
        if(playerCheckCollision(p, iw, EntityType_spike) || playerCheckCollision(p, iw, EntityType_fruit) || (bossCollision && bossCollision->controller->boss.triggered))
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
            musicPlayOnce(assetsGetMusic(&iw->game, "gameOverMusic"), 1, &iw->game);
            
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
            p->velocity.y = 0;
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
        
        bool onVineLeft = playerCheckCollisionAtOffset(p, iw, EntityType_vine, -1, 0) != 0;
        bool onVineRight = playerCheckCollisionAtOffset(p, iw, EntityType_vine, 1, 0) != 0;
        
        if(onVineLeft || onVineRight)
            p->velocity.y = 2;//clamp(p->velocity.y, -2, 2);
        
        // Running
        if(checkKey(g, KEY_LEFT))
        {
            p->sprite.scale.x = -1;
            p->velocity.x -= 3;
            
            if(onVineRight && checkKey(g, KEY_JUMP))
            {
                p->velocity.y = -9;
                p->velocity.x = -15;
                soundPlay(assetsGetSound(&iw->game, "jump"), 1);
                onVineRight = false;
            }
        }
        
        if(checkKey(g, KEY_RIGHT))
        {
            p->sprite.scale.x = 1;
            p->velocity.x += 3;
            
            if(onVineLeft && checkKey(g, KEY_JUMP))
            {
                p->velocity.y = -9;
                p->velocity.x = 15;
                soundPlay(assetsGetSound(&iw->game, "jump"), 1);
                onVineLeft = false;
            }
        }
        
        if(onVineLeft)
                p->sprite.scale.x = 1;
        else if(onVineRight)
                p->sprite.scale.x = -1;
        
        // Change the hitbox for different facing directions
        if(p->sprite.scale.x == -1)
        {
            if(p->hitBox.x > 10.5f)
            {
                p->position.x += 1;
                p->hitBox.x = 9.0f;
                p->hitBox.w = 11.0f; 
            }
        }
        else
        {
            if(p->hitBox.x < 10.5f)
            {
                p->position.x -= 1;
                p->hitBox.x = 12.0f;
                p->hitBox.w = 11.0f; 
            }
        }
        
        // Perform sprite animation logic
        if(onVineLeft || onVineRight)
            spritePlayAnimation(&p->sprite, Animations_slide);
        else if(onGround)
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
    if((warp = playerCheckCollision(p, iw, EntityType_warp)))
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