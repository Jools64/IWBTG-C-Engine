typedef enum GameState
{
    GameState_menu,
    GameState_inGame,
    GameState_gameOver
} GameState;

typedef struct SaveState
{
    Vector2f playerPosition;
    Vector2i room;
    int deaths;
    double time;
} SaveState;

typedef struct Level
{
    Grid entities;
    Grid ground;
    Grid controllers;
    Entity* entityMap[MAP_WIDTH][MAP_HEIGHT];
} Level;

#define TEXT_INPUT_MAX_LENGTH 4096
typedef struct TextInput
{
    char text[TEXT_INPUT_MAX_LENGTH];
    int length, backspaceFrames;
    bool active;
    Vector2f position;
} TextInput;

typedef struct Iwbtg
{
    Game game;
    Player player;
    
    //Vector2f checkPointPosition;
    //int checkPointRoom;
    SaveState saveState;
    
    Texture* blockTexture;
    Texture* backgroundTexture;
    Texture* cloudsTexture;
    Texture* gameOverTexture;
    Texture* titleTexture;
    
    Sprite gameOverSprite;
    
    Level level;
    Editor editor;
    
    Font fontSmall;
    
    Menu mainMenu, loadMenu;
    Menu* activeMenu;
    
    Entity entities[MAX_ENTITIES];
    Entity* entityDrawOrder[MAX_ENTITIES];
    int lastEntityPosition, entityDrawCount,
        activeSaveSlot, frameCount;
    Vector2i room;
    
    float time;
    
    GameState state;
    float gameOverTimer;
    
    TextInput textInput;
    
    Vector2f debugDrawPosition;
} Iwbtg;

char* getCurrentMapName(Iwbtg* iw, char* string, int stringLength);

Entity* getFirstFreeEntity(Iwbtg* iw);
int entityCount(Iwbtg* iw, EntityType type);
Entity* createEntity(Iwbtg* iw, EntityType type, float x, float y);
Entity* createParticle(Iwbtg* iw, Texture* texture, float x, float y, 
                       float hSpeed, float vSpeed, float life);
void destroyEntity(Entity* e);
void destroyAllEntities(Iwbtg* iw);
void loadMap(Iwbtg* iw, char* file);
void saveMap(Iwbtg* iw, char* file);
void loadGame(Iwbtg* iw);
void saveGame(Iwbtg* iw, bool position);
bool entityCheckPlayerCollision(Entity* e, Player* p);
bool rectangleCheckCollision(Rectanglef* r, Iwbtg* iw, EntityType type);
Entity* playerCheckCollision(Player* p, Iwbtg* iw, EntityType type);
Entity* playerCheckCollisionAtOffset(Player* p, Iwbtg* iw, EntityType type, int x, int y);
bool rectangleIsCollidingWithGround(Rectanglef* r, Iwbtg* iw, float offsetX, float offsetY);
bool playerIsCollidingWithGround(Player* p, Iwbtg* iw, float offsetX, float offsetY);
void playerInit(Player* p, float x, float y, Iwbtg* iw);
void playerUpdate(Player* p, Iwbtg* iw);
void iwbtgInit(Iwbtg* iw);
void iwbtgLoad(Iwbtg* iw);
void iwbtgUpdate(Iwbtg* iw);
int compareEntitiesByDepth(const void* a, const void* b);
bool checkAdjacentBlock(Iwbtg* iw, Entity* e, int x, int y);
void iwbtgDraw(Iwbtg* iw);