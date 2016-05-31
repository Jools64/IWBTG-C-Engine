#ifndef GAME_H
#define GAME_H

#define MAX_ELEMENTS_PER_RENDER_BATCH 200

#ifdef OPENGL

    typedef struct RenderBatch
    {
        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint uvBuffer;
        
        int vertexDataOffset;
        GLfloat vertexData[MAX_ELEMENTS_PER_RENDER_BATCH * 2];
        int indexDataOffset;
        GLfloat indexData[MAX_ELEMENTS_PER_RENDER_BATCH];
    } RenderBatch;

    typedef struct Shader
    {
        GLuint id;
        GLint inVertexPos;
        GLint inTexturePos;
        GLint uTextureSampler;
    } Shader;

    void renderBatchInit(RenderBatch* s)
    {
        GLfloat vertexData[] = 
        {
            -0.5f, -0.5f,
             0.5f, -0.5f,
            -0.5f,  0.5f,
             0.5f,  0.5f,
        };
        
        GLfloat uvData[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
        };
        
        GLuint indexData[] = { 0, 1, 2, 1, 2, 3 };
        
        glGenBuffers(1, &s->vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, s->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
        
        glGenBuffers(1, &s->uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, s->uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), uvData, GL_STATIC_DRAW);
        
        glGenBuffers(1, &s->indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLfloat), indexData, GL_STATIC_DRAW);
    }

    void renderBatchClear(RenderBatch* r)
    {
        
    }

    void renderBatchDrawTexture(RenderBatch* r, Texture* t, int x, int y)
    {
        
    }

#endif
    
typedef struct Game
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext* glContext;
    
    Vector2i size, windowSize;
    float scale;
    bool running, fullScreen;
    int lastUpdateTime;
    double frameTimer;
    
    MemoryPool globalMemory, levelMemory, frameMemory;
    
    Assets assets;
    Input input;
    KeyBindings keyBindings;
    Vector2f camera;
    Font font;
    Music* playing;

#ifdef OPENGL
    Shader defaultShader;
    RenderBatch renderBatch;
#endif
} Game;

bool gameInit(Game* g, char* title, int width, int height, float scale);
void gameHandleEvents(Game* g);

#endif // GAME_H