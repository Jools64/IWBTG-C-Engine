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
    
}

int assetsHash(char* string)
{
    int hash = 0;
    int length = strlen(string);
    
    for(int i = 0; i < length; ++i)
        hash += string[i] * (i*65);
    
    return hash % ASSET_ARRAY_SIZE;
}

void assetsAddTexture(Assets* as, Game* g, Texture texture, char* name)
{
    Asset* asset = (Asset*) memoryPoolAllocate(&g->globalMemory, sizeof(Asset));
    asset->name = cloneString(g, name);
    asset->texture = texture;
    asset->next = null;
    
    if(!as->assetArray[assetsHash(name)])
        as->assetArray[assetsHash(name)] = asset;
    else
    {
        Asset* a = as->assetArray[assetsHash(name)];
        while(a->next != null)
        {
            a = a->next;
            printf("Loopin\n");
        }
        a->next = asset;
    }
}

Texture* assetsGetTexture(Game* g, char* name)
{
    Assets* as = &g->assets;
    if(as->assetArray[assetsHash(name)])
    {
        Asset* a = as->assetArray[assetsHash(name)];
        while(a)
        {
            if(strcmp(a->name, name) == 0)
                return &a->texture;
            a = a->next;
        }
    }
    
    printf("Error: Asset not found!\n");
        return null;
}

void assetsLoadTexture(Game* g, char* filePath, char*name)
{
    Texture texture = textureLoad(g, filePath);
    assetsAddTexture(&g->assets, g, texture, name);
}
