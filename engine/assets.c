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
        texture.data = SDL_CreateTextureFromSurface(game->renderer, surface);
        texture.surface = surface;
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

int assetsHash(char* string)
{
    int hash = 0;
    int length = strlen(string);
    
    for(int i = 0; i < length; ++i)
        hash += string[i] * (i*65);
    
    return hash % ASSET_ARRAY_SIZE;
}

void assetsAdd(Assets* as, Asset* asset, Game* g)
{
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
    asset->name = cloneString(g, name);
    asset->texture = texture;
    asset->next = null;
    asset->type = AssetType_texture;
    
    assetsAdd(as, asset, g);
}

void assetsAddSound(Assets* as, Game*g, Sound sound, char* name)
{
    Asset* asset = (Asset*) memoryPoolAllocate(&g->globalMemory, sizeof(Asset));
    asset->name = cloneString(g, name);
    asset->sound = sound;
    asset->next = null;
    asset->type = AssetType_sound;
    
    assetsAdd(as, asset, g);
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
}

Texture* assetsGetTexture(Game* g, char* name)
{
    Asset* a = assetsGetAsset(g, name);
    if(a->type == AssetType_texture)
        return &a->texture;
    else
    {
        printf("Error: Asset \"%s\" is not a texture!\n", a->name);
        return 0;
    }
    
    printf("Error: Asset not found!\n");
        return null;
}

Sound* assetsGetSound(Game* g, char* name)
{
    Asset* a = assetsGetAsset(g, name);
    if(a->type == AssetType_sound)
        return &a->sound;
    else
    {
        printf("Error: Asset \"%s\" is not a sound!\n", a->name);
        return 0;
    }
    
    printf("Error: Asset not found!\n");
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
