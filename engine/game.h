#ifndef GAME_H
#define GAME_H

typedef struct Game
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    
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
} Game;

bool gameInit(Game* g, char* title, int width, int height, float scale);
void gameHandleEvents(Game* g);

#endif // GAME_H