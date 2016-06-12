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
    BossActionType_aimedProjectile,
    BossActionType_projectile,
    BossActionType_teleport
}

typedef struct BossAction
{
    Vector2f position;
    float time, speed, direction;
    int count;
}

typedef struct ControllerBoss
{
    int maxHealth, health;
    BossAction actionQueue[16];
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

void entitySetControllerFromTypeIndex(Entity* e, int typeIndex, Iwbtg* iw);
void controllerUpdate(Controller* c, Entity* e, Iwbtg* iw, float dt);
void addChainLink(Level* l, unsigned char chainLinks[MAP_WIDTH][MAP_HEIGHT], 
                  int x, int y, Controller** joinedController);
void resolveChain(Entity* e, Iwbtg* iw);