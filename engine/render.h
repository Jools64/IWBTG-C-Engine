#ifndef RENDERING_H
#define RENDERING_H

typedef struct Color
{
    float r, g, b, a;
} Color;

typedef struct
{
    SDL_Surface* surface;
    Vector2i size;
#ifdef OPENGL
    GLuint id;
#else
    SDL_Texture* data;
#endif
} Texture;

#define FRAMES_PER_ANIMATION 32

typedef struct 
{
    int frames[FRAMES_PER_ANIMATION];
    int frameCount;
    float frameRate;
    bool defined;
} Animation;

#define ANIMATIONS_PER_SPRITE 8

typedef struct
{
    Texture* texture;
    Vector2i size;
    Vector2f scale, origin;
    float angle, frameRate, frameTimer, alpha;
    Color color;
    Animation* currentAnimation;
    Animation animations[ANIMATIONS_PER_SPRITE];
    int frame;
    bool visible, additiveBlend;
} Sprite;

typedef struct
{
    char layout[256];
    int letterWidth[256];
    int lineSpacing, letterSpacing;
    Sprite sprite;
    bool singleCase;
} Font;

void renderBegin(Game* game);
void renderEnd(Game* game);

void rectangleDraw(Game* g, float x, float y, float w, float h, 
                   float cr, float cg, float cb, float ca);

void textureDrawExt(Game* g, Texture* t, 
                    float x, float y, float w, float h,
                    float tX, float tY, float tW, float tH,
                    float originX, float originY,
                    float scaleX, float scaleY, float angle, 
                    float alpha, bool additiveBlend, Color color);
void spriteInit(Sprite* s, Texture* t, float width, float height);
void spriteUpdate(Sprite* s, float delta);
void spriteDraw(Game* g, Sprite* s, float x, float y);
void textureDraw(Game* g, Texture* t, float x, float y);
void fontInit(Font* font, Texture* texture, int letterWidth, int letterHeight,
              char* layout);
void fontSetLetterWidth(Font* font, char letter, int letterWidth);
void fontSetLettersWidth(Font* font, char* letters, int letterWidth);
void fontSetAllLetterWidth(Font* font, int letterWidth);
void drawText(Game* g, Font* f, char* text, float x, float y);
void drawTextCentered(Game* g, Font* f, char* text, float x, float y);
Vector2f getTextCharPosition(Font* f, char* text, int character);
Vector2f getTextSize(Font* f, char* text);
float getTextLineWidth(Font* f, char* text);
Color makeColor(float r, float g, float b, float a);

#endif // RENDERING_H