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

#define SCRIPT_MAX_LENGTH 1024

typedef struct Script
{
    char text[SCRIPT_MAX_LENGTH];
    bool inUse;
    Vector2i position;
} Script;

typedef struct Background
{
    Texture* texture;
    Vector2f position, speed;
    bool loopX, loopY, enabled;
} Background;

#define MAX_BACKGROUNDS_PER_LEVEL 8
#define MAX_SCRIPTS_PER_LEVEL 32
#define MAX_LEVEL_PROPERTIES_SCRIPT_LENGTH 512
typedef struct Level
{
    // This structure represents the contents of a map file.
    Grid entities;
    Grid ground;
    Script scripts[32];
    Grid controllers;
    char propertiesScript[MAX_LEVEL_PROPERTIES_SCRIPT_LENGTH];
    // TODO: These shouldn't be in here...
} Level;

typedef struct TextInput
{
    char* text;
    int textMaxLength;
    int length, backspaceFrames, leftFrames, rightFrames, cursorPosition;
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
    
    Entity* entityMap[MAP_WIDTH][MAP_HEIGHT];
    Entity* boss;
    Background backgrounds[MAX_BACKGROUNDS_PER_LEVEL];
    Texture* blockTexture;
    
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

bool playerIsCollidingWithGround(Player* p, Iwbtg* iw, float offsetX, float offsetY);
void playerInit(Player* p, float x, float y, Iwbtg* iw);
void playerUpdate(Player* p, Iwbtg* iw);
void iwbtgInit(Iwbtg* iw);
void iwbtgLoad(Iwbtg* iw);
void iwbtgUpdate(Iwbtg* iw);
int compareEntitiesByDepth(const void* a, const void* b);
bool checkAdjacentBlock(Iwbtg* iw, Entity* e, int x, int y);
void iwbtgDraw(Iwbtg* iw);

Script* levelGetScriptAtPosition(Level* l, int x, int y);
Script* levelAddScript(Level* l, int x, int y);
void levelRemoveScript(Level* l, int x, int y);
void textInputEditString(TextInput* ti, char* string, int stringMaxLength);
void textInputInit(TextInput* ti);
void textInputUpdate(TextInput* ti, Iwbtg* iw);
void textInputDraw(TextInput* ti, Iwbtg* iw);