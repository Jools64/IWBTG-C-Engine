typedef enum BossActionType
{
    BossActionType_wait,
    BossActionType_move,
    BossActionType_projectileBurst,
    BossActionType_playMusic,
    BossActionType_projectileRandomSpray,
    BossActionType_projectileAimed
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

typedef struct BossActionProjectileRandomSpray
{
    float time, speed, previousDirection;
    int projectileEntityType, soundCounter;
} BossActionProjectileRandomSpray;

typedef struct BossActionProjectileAimied
{
    float speed;
    int projectileEntityType;
} BossActionProjectileAimed;

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
        BossActionProjectileRandomSpray projectileRandomSpray;
        BossActionProjectileAimed projectileAimed;
    };
} BossAction;

// TODO: Boss controller should have its data moved out of the controller system as
//       the data structure is way too large.
#define MAX_ACTIONS_PER_BOSS 64
typedef struct ControllerBoss
{
    int maxHealth, health;
    bool initialized, triggered;
    BossAction actionQueue[MAX_ACTIONS_PER_BOSS];
    BossAction* activeActions[MAX_ACTIONS_PER_BOSS];
    int actionQueueHead, actionQueueTail;
} ControllerBoss;

int bossGetNextActionIndex(int currentAction);
bool bossIsActionQueueFull(ControllerBoss* b);
BossAction* bossAddAction(Entity* e, BossActionType type);