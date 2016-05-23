bool gameInit(Game* g, char* title, int width, int height, float scale)
{
    Vector2i size = { width, height };
    g->size = size;
    g->scale = scale;
    g->windowSize.x = size.x * scale;
    g->windowSize.y = size.y * scale;
    
    initKeyBindings(&g->keyBindings);

    memoryPoolInit(&g->globalMemory, KB(64));
    memoryPoolInit(&g->levelMemory, KB(16));
    memoryPoolInit(&g->frameMemory, KB(16));
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }
    
    g->window = 
        SDL_CreateWindow(title, 
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         width * scale, height * scale, 
                         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                         
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
                         
    if (!g->window)
    {
	    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
	    SDL_Quit();
	    return 1;
    }
    
    g->renderer = 
        SDL_CreateRenderer(g->window, -1, 
                           SDL_RENDERER_ACCELERATED 
                           | SDL_RENDERER_PRESENTVSYNC);
    if (!g->renderer)
    {
	    SDL_DestroyWindow(g->window);
	    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
	    SDL_Quit();
	    return 1;
    }
    
    SDL_RenderSetScale(g->renderer, scale, scale);
    
    if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) 
    {
        printf("IMG_Init: Failed to init required png support!\n");
        printf("IMG_Init: %s\n", IMG_GetError());
    }
    
    g->running = true;
    
    printf("Game initialized\n");
    return true;
}

void gameFullscreenToggle(Game* g)
{
    if(g->fullScreen)
        SDL_SetWindowFullscreen(g->window, 0);
    else
        SDL_SetWindowFullscreen(g->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    g->fullScreen = !g->fullScreen;
}

void gameHandleEvents(Game* g)
{
    SDL_Event e;
    memset(&g->input.keysPressed, 0, MAX_KEYS);
    memset(&g->input.keysReleased, 0, MAX_KEYS);
    memset(&g->input.mouseButtonsPressed, 0, MAX_MOUSE_BUTTONS);
    memset(&g->input.mouseButtonsReleased, 0, MAX_MOUSE_BUTTONS);
    
    g->input.text[0] = '\0';
    
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_QUIT:
                g->running = false;
                break;
                
            case SDL_KEYDOWN:
                if(e.key.repeat == 0)
                {
                    g->input.keys[e.key.keysym.sym & 255] = true;
                    g->input.keysPressed[e.key.keysym.sym & 255] = true;
                }
                break;
                
            case SDL_KEYUP:
                g->input.keys[e.key.keysym.sym & 255] = false;
                g->input.keysReleased[e.key.keysym.sym & 255] = true;
                break;
                
            case SDL_WINDOWEVENT:
                if(e.window.event == SDL_WINDOWEVENT_RESIZED 
                || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    int ww = e.window.data1,
                        wh = e.window.data2;
                    g->windowSize.x = ww;
                    g->windowSize.y = wh;
                    SDL_RenderSetScale(g->renderer, 
                                       (float)ww / g->size.x, 
                                       (float)wh / g->size.y);
                }
                break;
                
            case SDL_MOUSEMOTION:
                setMousePosition(g, e.motion.x, e.motion.y);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                g->input.mouseButtons[e.button.button] = true;
                g->input.mouseButtonsPressed[e.button.button] = true;
                setMousePosition(g, e.button.x, e.button.y);
                break;
                
            case SDL_MOUSEBUTTONUP:
                g->input.mouseButtons[e.button.button] = false;
                g->input.mouseButtonsReleased[e.button.button] = true;
                setMousePosition(g, e.button.x, e.button.y);
                break;
                
            case SDL_MOUSEWHEEL:
                break;
                
            case SDL_TEXTINPUT:
                strcat(g->input.text, e.text.text);
                break;
        }
    }
}