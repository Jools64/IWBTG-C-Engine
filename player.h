typedef struct Player
{
    Vector2f position, velocity;
    Sprite sprite;
    Rectanglef hitBox;
    float runSpeed, gravity,
          jumpSpeed, doubleJumpSpeed,
          jumpDampening, maxFallSpeed;
    bool doubleJumpAvailible, dead;
} Player;

void playerInit(Player* p, float x, float y, Iwbtg* iw);
void playerUpdate(Player* p, Iwbtg* iw);