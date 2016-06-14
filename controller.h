typedef struct Level Level;
typedef struct Entity Entity;
typedef struct Iwbtg Iwbtg;

typedef enum
{
    ControllerType_none,
    ControllerType_trap,
    ControllerType_inOut,
    ControllerType_bounce,
    ControllerType_chain,
    ControllerType_boss,
} ControllerType;

typedef struct ControllerTrap
{
    float direction;
    float speed;
    float activationDistance;
    float activationWidth;
    int activationFrame;
    unsigned char activated;
} ControllerTrap;

typedef struct ControllerInOut
{
    float speed;
    float distance;
    float direction;
    float interval;
    float timer;
    Vector2f basePosition;
} ControllerInOut;

typedef enum BossActionType
{
    BossActionType_wait,
    BossActionType_move,
    BossActionType_projectileBurst,
    BossActionType_playMusic
} BossActionType;

typedef struct BossActionWait
{
    float time;
} BossActionWait;

typedef struct BossActionMove
{
    float time;
    Vector2f destination, start;
} BossActionMove;

typedef struct BossActionProjectileBurst
{
    int count, repeat;
    float speed, interval, direction, rotation, shotTimer;
    int projectileEntityType;
} BossActionProjectileBurst;

typedef struct BossActionPlayMusic
{
    Music* music;
} BossActionPlayMusic;

typedef struct BossAction
{
    BossActionType type;
    float time;
    bool active, initialized, parallel;
    union
    {
        BossActionWait wait;
        BossActionMove move;
        BossActionProjectileBurst projectileBurst;
        BossActionPlayMusic playMusic;
    };
} BossAction;

// TODO: Boss controller should be moved out of controller system as
//       the data structure is quite large.
#define MAX_ACTIONS_PER_BOSS 32
typedef struct ControllerBoss
{
    int maxHealth, health;
    bool initialized, triggered;
    BossAction actionQueue[MAX_ACTIONS_PER_BOSS];
    BossAction* activeActions[MAX_ACTIONS_PER_BOSS];
    int actionQueueHead, actionQueueTail;
} ControllerBoss;

typedef struct Controller Controller;
typedef struct Controller
{
    ControllerType type;
    union
    {
        ControllerTrap trap;
        ControllerInOut inOut;
        ControllerBoss boss;
    };
    unsigned char hasChains;
} Controller;

int bossGetNextActionIndex(int currentAction);
bool bossIsActionQueueFull(ControllerBoss* b);
BossAction* bossAddAction(Entity* e, BossActionType type);

void entitySetControllerFromTypeIndex(Entity* e, int typeIndex, Iwbtg* iw);
void controllerUpdate(Controller* c, Entity* e, Iwbtg* iw, float dt);
void addChainLink(Level* l, unsigned char chainLinks[MAP_WIDTH][MAP_HEIGHT], 
                  int x, int y, Controller** joinedController);
void resolveChain(Entity* e, Iwbtg* iw);