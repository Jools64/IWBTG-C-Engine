Texture textureLoad(Game* game, char* filePath)
{
    Texture texture = {0};
    
    SDL_Surface* surface = IMG_Load(filePath);
    if(!surface)
    {
        printf("Warning: \"%s\" not found\n", filePath);
        return texture;
    }
    else
    {
        texture.size.x = surface->w;
        texture.size.y = surface->h;
        texture.surface = surface;
        
        #ifdef OPENGL
            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);
            if(surface->format->BytesPerPixel == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.size.x, texture.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.surface->pixels);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.size.x, texture.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.surface->pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        #else
            texture.data = SDL_CreateTextureFromSurface(game->renderer, surface);
        #endif
        return texture;
    }
}

Sound soundLoad(Game* game, char* filePath)
{
    Sound sound = {0};
    if(!(sound.data = Mix_LoadWAV(filePath)))
        printf("Warning: \"%s\" could not be loaded: %s\n", filePath, Mix_GetError());
    return sound;
}

Music musicLoad(Game* game, char* filePath)
{
    Music music = {0};
    if(!(music.data = Mix_LoadMUS(filePath)))
        printf("Warning: \"%s\" could not be loaded: %s\n", filePath, Mix_GetError());
    return music;
}

int assetsHash(char* string)
{
    int hash = 0;
    int length = strlen(string);
    
    for(int i = 0; i < length; ++i)
        hash += string[i] * (i*65);
    
    return hash % ASSET_ARRAY_SIZE;
}

void assetsAdd(Assets* as, Asset* asset, Game* g, char* name)
{
    asset->name = cloneString(g, name);
    asset->next = null;
    
    int hash = assetsHash(asset->name);
    if(!as->assetArray[hash])
        as->assetArray[hash] = asset;
    else
    {
        Asset* a = as->assetArray[hash];
        while(a->next != null)
            a = a->next;
        a->next = asset;
    }
}

void assetsAddTexture(Assets* as, Game* g, Texture texture, char* name)
{
    Asset* asset = (Asset*) memoryPoolAllocate(&g->globalMemory, sizeof(Asset));
    asset->texture = texture;
    asset->type = AssetType_texture;
    assetsAdd(as, asset, g, name);
}

void assetsAddSound(Assets* as, Game*g, Sound sound, char* name)
{
    Asset* asset = (Asset*) memoryPoolAllocate(&g->globalMemory, sizeof(Asset));
    asset->sound = sound;
    asset->type = AssetType_sound;
    
    assetsAdd(as, asset, g, name);
}

void assetsAddMusic(Assets* as, Game*g, Music music, char* name)
{
    Asset* asset = (Asset*) memoryPoolAllocate(&g->globalMemory, sizeof(Asset));
    asset->music = music;
    asset->type = AssetType_music;
    
    assetsAdd(as, asset, g, name);
}

Asset* assetsGetAsset(Game* g, char* name)
{
    Assets* as = &g->assets;
    if(as->assetArray[assetsHash(name)])
    {
        Asset* a = as->assetArray[assetsHash(name)];
        while(a)
        {
            if(strcmp(a->name, name) == 0)
                return a;
            a = a->next;
        }
    }
    return 0;
}

Texture* assetsGetTexture(Game* g, char* name)
{
    Asset* a = assetsGetAsset(g, name);
    if(a)
    {
        if(a->type == AssetType_texture)
            return &a->texture;
        else
        {
            printf("Error: Asset \"%s\" is not a texture!\n", a->name);
            return 0;
        }
    }
    
    printf("Error: Asset not found! \"%s\"\n", name);
        return null;
}

Sound* assetsGetSound(Game* g, char* name)
{
    Asset* a = assetsGetAsset(g, name);
    if(a)
    {
        if(a->type == AssetType_sound)
            return &a->sound;
        else
        {
            printf("Error: Asset \"%s\" is not a sound!\n", a->name);
            return 0;
        }
    }
    
    printf("Error: Asset not found! \"%s\"\n", name);
        return null;
}

Music* assetsGetMusic(Game* g, char* name)
{
    Asset* a = assetsGetAsset(g, name);
    if(a)
    {
        if(a->type == AssetType_music)
            return &a->music;
        else
        {
            printf("Error: Asset \"%s\" is not music!\n", a->name);
            return 0;
        }
    }
    
    printf("Error: Asset not found! \"%s\"\n", name);
        return null;
}

void assetsLoadTexture(Game* g, char* filePath, char*name)
{
    Texture texture = textureLoad(g, filePath);
    assetsAddTexture(&g->assets, g, texture, name);
}

void assetsLoadSound(Game* g, char* filePath, char*name)
{
    Sound sound = soundLoad(g, filePath);
    assetsAddSound(&g->assets, g, sound, name);
}

void assetsLoadMusic(Game* g, char* filePath, char*name)
{
    Music music = musicLoad(g, filePath);
    assetsAddMusic(&g->assets, g, music, name);
}
