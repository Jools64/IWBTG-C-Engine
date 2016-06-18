typedef enum Animations
{
    Animations_default,
    Animations_stand,
    Animations_run,
    Animations_jump,
    Animations_fall,
    Animations_slide
} Animations;

typedef enum EntityType
{
    EntityType_spike,
    EntityType_smallSpike,
    EntityType_block,
    EntityType_save,
    EntityType_warp,
    EntityType_particle,
    EntityType_playerBullet,
    EntityType_fruit,
    EntityType_movingPlatform,
    EntityType_boss,
    EntityType_vine,
    EntityType_jumpRefresher,
} EntityType;

typedef struct Entity
{
    Vector2f position, velocity, acceleration;
    Sprite sprite;
    EntityType type;
    bool active;
    float spin, spinFriction, friction;
    float animationTimer;
    float depth;
    Controller* controller;
    Controller controllerData;
} Entity;

Entity* getFirstFreeEntity(Iwbtg* iw);
int entityCount(Iwbtg* iw, EntityType type);
Entity* createEntity(Iwbtg* iw, EntityType type, float x, float y);
Entity* createEntityFromTypeIndex(Iwbtg* iw, int typeIndex, int x, int y);
void destroyEntity(Entity* e);
void destroyAllEntities(Iwbtg* iw);
void entityUpdate(Entity* e, Iwbtg* iw, float dt);
bool checkAdjacentBlock(Iwbtg* iw, Entity* e, int x, int y);
void entityDraw(Entity* e, Iwbtg* iw);