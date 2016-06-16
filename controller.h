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
    ControllerType_booCircle
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
    bool ease;
    Vector2f basePosition;
} ControllerInOut;

#define MAX_ENTITIES_PER_BOO_CIRCLE 16
typedef struct ControllerBooCircle
{
    float speed, distance, count, timer;
    Entity* entities[MAX_ENTITIES_PER_BOO_CIRCLE];
    Vector2f origin;
} ControllerBooCircle;

#include "boss.h"

typedef struct Controller Controller;
typedef struct Controller
{
    ControllerType type;
    union
    {
        ControllerTrap trap;
        ControllerInOut inOut;
        ControllerBoss boss;
        ControllerBooCircle booCircle;
    };
    unsigned char hasChains;
} Controller;

void entitySetControllerFromTypeIndex(Entity* e, int typeIndex, Iwbtg* iw);
void controllerUpdate(Controller* c, Entity* e, Iwbtg* iw, float dt);
void addChainLink(Level* l, unsigned char chainLinks[MAP_WIDTH][MAP_HEIGHT], 
                  int x, int y, Controller** joinedController, Iwbtg* iw);
void resolveChain(Entity* e, Iwbtg* iw);