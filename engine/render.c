void renderBegin(Game* game)
{
    SDL_SetRenderDrawColor(game->renderer, 30, 80, 200, 255);
    SDL_RenderClear(game->renderer);
}

void renderEnd(Game* game)
{
    SDL_RenderPresent(game->renderer);
}

void rectangleDraw(Game* g, float x, float y, float w, float h, 
                   float cr, float cg, float cb, float ca)
{
    SDL_Rect rect = { (int)x, (int)y, (int)w, (int)h };
    SDL_SetRenderDrawBlendMode(g->renderer,
                               SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(g->renderer, 
                           clampi((cr * 255), 0, 255), 
                           clampi((cg * 255), 0, 255), 
                           clampi((cb * 255), 0, 255), 
                           clampi((ca * 255), 0, 255));
    SDL_RenderFillRect(g->renderer, &rect);
}

void textureDrawExt(Game* g, Texture* t, 
                    float x, float y, float w, float h,
                    float tX, float tY, float tW, float tH,
                    float originX, float originY,
                    float scaleX, float scaleY, float angle, float alpha,
                    bool additiveBlend)
{
    float scaleOffsetX = fabs(scaleX) * w * (originX / w);
    float scaleOffsetY = fabs(scaleY) * h * (originY / h);
    
    SDL_Rect dest = { 
        x - scaleOffsetX - roundf(g->camera.x) + originX, 
        y - scaleOffsetY - roundf(g->camera.y) + originY, 
        w *= fabs(scaleX), 
        h *= fabs(scaleY) 
    };
    
    int flip = 0;
    if(scaleX < 0)
        flip = flip | SDL_FLIP_HORIZONTAL;
    if(scaleY < 0)
        flip = flip | SDL_FLIP_VERTICAL;
    
    if(alpha > 1)
        alpha = 1;
    if(alpha < 0)
        alpha = 0;
    
    SDL_SetTextureAlphaMod(t->data, alpha * 255);
    
    if(additiveBlend)
        SDL_SetTextureBlendMode(t->data, SDL_BLENDMODE_ADD);
    else
        SDL_SetTextureBlendMode(t->data, SDL_BLENDMODE_BLEND);
    
    SDL_Rect source = { tX, tY, tW, tH };
    SDL_Point center = { originX * scaleX, originY * scaleY };
    SDL_RenderCopyEx(g->renderer, t->data, &source, &dest, 
                     angle * 180 / PI, &center, flip);
} 

void spriteInit(Sprite* s, Texture* t, float width, float height)
{
    Sprite clear = {0};
    *s = clear;
    s->texture = t;
    s->size.x = width;
    s->size.y = height;
    s->origin.x = width / 2;
    s->origin.y = height / 2;
    s->scale.x = 1;
    s->scale.y = 1;
    s->visible = true;
    s->alpha = 1;
    s->additiveBlend = false;
}

void spriteUpdate(Sprite* s, float delta)
{
    if(s->currentAnimation)
    {
        s->frameTimer += delta * s->frameRate;
        if(s->frameTimer >= s->currentAnimation->frameCount)
            s->frameTimer -= s->currentAnimation->frameCount;
        s->frame = s->currentAnimation->frames[(int)s->frameTimer];
    }
}

void spriteAddAnimation(Sprite* s, int index, int* frames, int frameCount, float frameRate)
{
    if(index >= ANIMATIONS_PER_SPRITE - 1 || index < 0)
        printf("Error: Impossible animation index.\n");
    else if(frameCount >= FRAMES_PER_ANIMATION)
        printf("Error: Animation is too long to be defined.\n");
    else
    {
        Animation* a = &s->animations[index];
        memcpy(a->frames, frames, frameCount * sizeof(int));
        a->frameCount = frameCount;
        a->frameRate = frameRate;
        a->defined = true;
    }
}

void spritePlayAnimation(Sprite* s, int index)
{
    if(index >= ANIMATIONS_PER_SPRITE - 1 || index < 0)
        printf("Error: Impossible animation index.\n");
    else
    {
        Animation* a = &s->animations[index];
        if(!a->defined)
            printf("Error: Trying to play undefined animation.\n");
        else
        {
            if(s->currentAnimation != a)
            {
                s->frameTimer = 0;
                s->frameRate = a->frameRate;
                s->currentAnimation = a;
            }
        }
    }
}

void spriteDraw(Game* g, Sprite* s, float x, float y)
{
    if(s->visible)
    {
        int tX = (s->frame * s->size.x) % s->texture->size.x,
            tY = ((s->frame * s->size.x) / s->texture->size.x) * s->size.y;
              
        textureDrawExt(g, s->texture, x, y, s->size.x, s->size.y,
                       tX, tY, s->size.x, s->size.y,
                       s->origin.x, s->origin.y,
                       s->scale.x, s->scale.y, s->angle, s->alpha, s->additiveBlend);
    }
}

void textureDraw(Game* g, Texture* t, float x, float y)
{
    SDL_Rect dest = { 
        x - roundf(g->camera.x), 
        y - roundf(g->camera.y), 
        t->size.x, 
        t->size.y
    };
    
    SDL_RenderCopy(g->renderer, t->data, null, &dest);
} 

void fontInit(Font* font, Texture* texture, int letterWidth, int letterHeight,
              char* layout)
{
    memset(font->layout, 0, 255);
    memset(font->letterWidth, 0, 255);
    font->letterWidth[(int)' '] = letterWidth;
    font->lineSpacing = letterHeight + 3;
    font->letterSpacing = 2;
    int length = strlen(layout); 
    if(length < 255)
    {
        spriteInit(&font->sprite, texture, letterWidth, letterHeight);
        for(int i = 0; i < length; ++i)
        {
            font->layout[(int)layout[i]] = i;
            font->letterWidth[(int)layout[i]] = letterWidth; 
        }
    }
    else
        printf("Error: font layout is too long\n");
}

void fontSetLetterWidth(Font* font, char letter, int letterWidth)
{
    font->letterWidth[(int)letter] = letterWidth; 
}

void fontSetLettersWidth(Font* font, char* letters, int letterWidth)
{
    int length = strlen(letters); 
    if(length < 255)
        for(int i = 0; i < length; ++i)
            font->letterWidth[(int)letters[i]] = letterWidth; 
    else
        printf("Error: Setting the width of too many characters\n");
}

void fontSetAllLetterWidth(Font* font, int letterWidth)
{
    for(int i = 0; i < 255; ++i)
        font->letterWidth[i] = letterWidth;
}

void drawText(Game* g, Font* f, char* text, float x, float y)
{
    int offsetX = 0, 
        offsetY = 0, 
        length = strlen(text);
    
    if(f == 0)
        f = &g->font;
    
    for(int i = 0; i < length; ++i)
    {
        char c = f->layout[(int)text[i]];
        if(text[i] == '\n')
        {
            offsetX = 0;
            offsetY += f->lineSpacing;
            continue;
        }
        if(text[i] != ' ')
        {
            f->sprite.frame = c;
            spriteDraw(g, &f->sprite, x + offsetX, y + offsetY);
        }
        offsetX += f->letterWidth[(int)text[i]] + f->letterSpacing;
    }
}