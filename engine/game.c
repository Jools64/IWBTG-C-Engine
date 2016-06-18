#ifdef OPENGL

    void printProgramLog(GLuint program)
    {
        if(glIsProgram(program))
        {
            int length = 0;
            int maxLength = length;
            
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            char* log = malloc(sizeof(char) * maxLength);
            glGetProgramInfoLog(program, maxLength, &length, log);
            if(length > 0)
                printf("%s\n", log);
            free(log);
        }
        else
            printf("Name %d is not a program\n", program);
    }

    void printShaderLog(GLuint shader)
    {
        if(glIsShader(shader))
        {
            int length = 0;
            int maxLength = length;
            
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
            char* log = malloc(sizeof(char) * maxLength);
            glGetShaderInfoLog(shader, maxLength, &length, log);
            if(length > 0)
                printf("%s\n", log);
            free(log);
        }
        else
            printf("Name %d is not a shader\n", shader);
    }

    bool compileShader(GLuint* programId, char* vertexSource, char* fragmentSource)
    {
        *programId = glCreateProgram();
        GLint compiled;
        
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
        glCompileShader(vertexShader);
        compiled = GL_FALSE;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
        if(compiled != GL_TRUE)
        {
            printf("Unable to compile vertex shader:\n");
            printShaderLog(vertexShader);
            return false;
        }
        else
            glAttachShader(*programId, vertexShader);
        
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
        glCompileShader(fragmentShader);
        compiled = GL_FALSE;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
        if(compiled != GL_TRUE)
        {
            printf("Unable to compile fragment shader:\n");
            printShaderLog(fragmentShader);
            return false;
        }
        else
            glAttachShader(*programId, fragmentShader);
        
        glLinkProgram(*programId);
        
        GLint success = GL_TRUE;
        glGetProgramiv(*programId, GL_LINK_STATUS, &success);
        if(success != GL_TRUE)
        {
            printf("Error linking shader program:\n");
            printProgramLog(*programId);
            return false;
        }
        
        return true;
    }

    bool initGl(Game* g)
    {
        char* vertexSource = loadText("assets/default.vert");
        char* fragmentSource = loadText("assets/default.frag");

        renderBatchInit(&g->renderBatch, g);
        
        bool compiled = compileShader(&g->defaultShader.id, vertexSource, fragmentSource);
        
        free(vertexSource);
        free(fragmentSource);
        
        if(compiled)
        {
            
            if((g->defaultShader.inVertexPos = glGetAttribLocation(g->defaultShader.id, "inVertexPos")) == -1)
                printf("Warning: Shader program does not contain \"inVertexPos\" variable.\n");
            
            if((g->defaultShader.inTexturePos = glGetAttribLocation(g->defaultShader.id, "inTexturePos")) == -1)
                printf("Warning: Shader program does not contain \"inTexturePos\" variable.\n");
            
            if((g->defaultShader.uTextureSampler = glGetUniformLocation(g->defaultShader.id, "uTextureSampler")) == -1)
                printf("Warning: Shader program does not contain \"uTextureSampler\" variable.\n");
            
        }
        else
            return false;
        
        return true;
    }

#endif

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
    
    #ifdef OPENGL
    
        g->window = 
        SDL_CreateWindow(title, 
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         width * scale, height * scale, 
                         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
                             
        if (!g->window)
        {
            printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        g->glContext = SDL_GL_CreateContext(g->window);
        if(g->glContext)
        {
            glewExperimental = GL_TRUE;
            GLenum glewError = glewInit();
            if( glewError != GLEW_OK )
            {
                printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
            }

            //Use Vsync
            if(SDL_GL_SetSwapInterval( 1 ) < 0)
            {
                printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
            }

            //Initialize OpenGL
            if(!initGl(g))
                printf("Unable to initialize OpenGL!\n");
            else
                printf("OpenGL initialized\n");
        }
        else
            printf("Could not create OpenGl context: %s\n", SDL_GetError());
        
    #else
    
        g->window = 
        SDL_CreateWindow(title, 
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         width * scale, height * scale, 
                         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                             
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
    
    #endif
    
    if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) 
    {
        printf("IMG_Init: Failed to init required png support!\n");
        printf("IMG_Init: %s\n", IMG_GetError());
    }
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512);//1024);
    
    g->running = true;
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); // Nearest Neighbor
    
    srand(time(0));
    
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
    
    g->input.frameVoid = false;
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
                    
                    #ifdef OPENGL
                    
                    #else
                        SDL_RenderSetScale(g->renderer, 
                                           (float)ww / g->size.x, 
                                           (float)wh / g->size.y);
                    #endif
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