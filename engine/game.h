#ifndef GAME_H
#define GAME_H

#define MAX_ELEMENTS_PER_RENDER_BATCH 1024
    
#ifdef OPENGL

    typedef struct Shader
    {
        GLuint id;
        GLint inVertexPos;
        GLint inTexturePos;
        GLint uTextureSampler;
    } Shader;

    typedef struct RenderBatch
    {
        Texture* texture, * boundTexture;
        Shader* shader, * boundShader;
        
        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint uvBuffer;
        int vertexCount, elementCount;
        
        int vertexDataOffset;
        GLfloat vertexData[MAX_ELEMENTS_PER_RENDER_BATCH * 8];
        int indexDataOffset;
        GLint indexData[MAX_ELEMENTS_PER_RENDER_BATCH * 6];
        int uvDataOffset;
        GLfloat uvData[MAX_ELEMENTS_PER_RENDER_BATCH * 8];
    } RenderBatch;

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

#ifdef OPENGL

    void renderBatchClear(RenderBatch* r)
    {
        r->vertexDataOffset = 0;
        r->uvDataOffset = 0;
        r->indexDataOffset = 0;
        r->vertexCount = 0;
        r->elementCount = 0;
    }

    void renderBatchInit(RenderBatch* r, Game* g)
    {
        renderBatchClear(r);
        
        r->texture = r->boundTexture = 0;
        r->shader = &g->defaultShader;
        r->boundShader = 0;
        
        glGenBuffers(1, &r->vertexBuffer);
        glGenBuffers(1, &r->uvBuffer);
        glGenBuffers(1, &r->indexBuffer);
    }

    void renderBatchFlush(RenderBatch* r, Game* g)
    {
        glBindBuffer(GL_ARRAY_BUFFER, r->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, r->vertexDataOffset * sizeof(GLfloat), r->vertexData, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, r->uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, r->uvDataOffset * sizeof(GLfloat), r->uvData, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, r->indexDataOffset * sizeof(GLint), r->indexData, GL_STREAM_DRAW);
        
        if(r->shader != r->boundShader)
        {
            if(r->boundShader != 0)
            {
                glDisableVertexAttribArray(g->defaultShader.inVertexPos);
                glDisableVertexAttribArray(g->defaultShader.inTexturePos);
                glUseProgram(0);
            }
            
            glUseProgram(r->shader->id);
            glEnableVertexAttribArray(r->shader->inVertexPos);
            glEnableVertexAttribArray(r->shader->inTexturePos);
            
            r->boundShader = r->shader;
        }
        
        
        glUniform1i(g->defaultShader.uTextureSampler, 0);

        if(r->texture != r->boundTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, r->texture->id);
            r->boundTexture = r->texture;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, g->renderBatch.vertexBuffer);
        glVertexAttribPointer(g->defaultShader.inVertexPos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, g->renderBatch.uvBuffer);
        glVertexAttribPointer(g->defaultShader.inTexturePos, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->renderBatch.indexBuffer);
        glDrawElements(GL_TRIANGLES, r->indexDataOffset, GL_UNSIGNED_INT, 0);

        renderBatchClear(r);
    }
    
    void renderBatchDrawTexture(RenderBatch* r, Texture* t, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, Game* g)
    {
        if(r->elementCount == MAX_ELEMENTS_PER_RENDER_BATCH || r->texture != t)
            renderBatchFlush(r, g);
        
        r->texture = t;
        
        float tw = (float)1 / t->size.x;
        float th = (float)1 / t->size.y;
        
        sx *= tw;
        sy *= th;
        sw *= tw;
        sh *= th;
        
        int vo = r->vertexDataOffset;
        int uo = r->uvDataOffset;
        int io = r->indexDataOffset;
        int vc = r->vertexCount;
        
        // Unrolled for debug build performance (Probably doesn't make a difference in release)
        
        r->vertexData[vo  ] = dx;
        r->vertexData[vo+1] = dy;
        r->vertexData[vo+2] = (dx + dw);
        r->vertexData[vo+3] = dy;
        r->vertexData[vo+4] = dx;
        r->vertexData[vo+5] = (dy + dh);
        r->vertexData[vo+6] = (dx + dw);
        r->vertexData[vo+7] = (dy + dh);
        
        r->uvData[uo  ] = sx;
        r->uvData[uo+1] = sy;
        r->uvData[uo+2] = sx + sw;
        r->uvData[uo+3] = sy;
        r->uvData[uo+4] = sx;
        r->uvData[uo+5] = sy + sh;
        r->uvData[uo+6] = sx + sw;
        r->uvData[uo+7] = sy + sh;
        
        r->indexData[io  ] = vc + 0;
        r->indexData[io+1] = vc + 1;
        r->indexData[io+2] = vc + 2;
        r->indexData[io+3] = vc + 1;
        r->indexData[io+4] = vc + 2;
        r->indexData[io+5] = vc + 3;
        
        r->vertexDataOffset += 8;
        r->uvDataOffset += 8;
        r->indexDataOffset += 6;
        r->vertexCount += 4;
        r->elementCount++;
    }
    
    void rotateVertex(float* x, float* y, float angle)
    {
        // Current angle and distance
        float d = sqrt((*x * *x) + (*y * *y));
        float a = atan2(*y, *x);
        
        a += angle;
        
        *x = cos(a) * d;
        *y = sin(a) * d;
    }
    
    void renderBatchDrawTextureExt(RenderBatch* r, Texture* t, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh,
                                   float originX, float originY, float angle, float scaleX, float scaleY, Game* g)
    {
        if(r->elementCount == MAX_ELEMENTS_PER_RENDER_BATCH || r->texture != t)
            renderBatchFlush(r, g);
        
        r->texture = t;
        
        // Texture size coeficients
        float tw = (float)1 / t->size.x;
        float th = (float)1 / t->size.y;
        
        sx *= tw;
        sy *= th;
        sw *= tw;
        sh *= th;
        
        // Rotation and scaling transformation
        float ox = dx + originX, 
              oy = dy + originY;
        
        float x1 = dx, 
              y1 = dy,
              x2 = dx + dw, 
              y2 = dy,
              x3 = dx,
              y3 = dy + dh,
              x4 = dx + dw,
              y4 = dy + dh;
       
       
        // Translate the quad
        x1 -= ox; x2 -= ox; x3 -= ox; x4 -= ox; y1 -= oy; y2 -= oy; y3 -= oy; y4 -= oy;
        
        // Scale the quad
        x1 *= scaleX; x2 *= scaleX; x3 *= scaleX; x4 *= scaleX; 
        y1 *= scaleY; y2 *= scaleY; y3 *= scaleY; y4 *= scaleY;
        
        // Rotate the quad
        rotateVertex(&x1, &y1, angle);
        rotateVertex(&x2, &y2, angle);
        rotateVertex(&x3, &y3, angle);
        rotateVertex(&x4, &y4, angle);
        
        // Translate the quad back to position
        x1 += ox; x2 += ox; x3 += ox; x4 += ox; y1 += oy; y2 += oy; y3 += oy; y4 += oy;
        
        int vo = r->vertexDataOffset;
        int uo = r->uvDataOffset;
        int io = r->indexDataOffset;
        int vc = r->vertexCount;
        
        r->vertexData[vo  ] = x1;
        r->vertexData[vo+1] = y1;
        r->vertexData[vo+2] = x2;
        r->vertexData[vo+3] = y2;
        r->vertexData[vo+4] = x3;
        r->vertexData[vo+5] = y3;
        r->vertexData[vo+6] = x4;
        r->vertexData[vo+7] = y4;
        
        r->uvData[uo  ] = sx;
        r->uvData[uo+1] = sy;
        r->uvData[uo+2] = sx + sw;
        r->uvData[uo+3] = sy;
        r->uvData[uo+4] = sx;
        r->uvData[uo+5] = sy + sh;
        r->uvData[uo+6] = sx + sw;
        r->uvData[uo+7] = sy + sh;
        
        r->indexData[io  ] = vc + 0;
        r->indexData[io+1] = vc + 1;
        r->indexData[io+2] = vc + 2;
        r->indexData[io+3] = vc + 1;
        r->indexData[io+4] = vc + 2;
        r->indexData[io+5] = vc + 3;
        
        r->vertexDataOffset += 8;
        r->uvDataOffset += 8;
        r->indexDataOffset += 6;
        r->vertexCount += 4;
        r->elementCount++;
    }

#endif

bool gameInit(Game* g, char* title, int width, int height, float scale);
void gameHandleEvents(Game* g);

#endif // GAME_H