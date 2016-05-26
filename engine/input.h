#define MAX_KEYS 255
#define MAX_MOUSE_BUTTONS 5
#define MAX_TEXT_LENGTH 256
typedef struct
{
    bool keysPressed[MAX_KEYS],
         keysReleased[MAX_KEYS],
         keys[MAX_KEYS],
         
         mouseButtons[MAX_MOUSE_BUTTONS],
         mouseButtonsPressed[MAX_MOUSE_BUTTONS],
         mouseButtonsReleased[MAX_MOUSE_BUTTONS];
         
    Vector2i mousePosition;
    
    char text[MAX_TEXT_LENGTH];
    bool frameVoid;
} Input;

#define MAX_KEY_BINDINGS 20
typedef enum
{
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_CONFIRM,
    KEY_CANCEL,
	KEY_MENU,
    KEY_SAVE_LEVEL,
    KEY_LOAD_LEVEL,
    KEY_CONSOLE,
    KEY_JUMP,
    KEY_TILE_PICKER,
    KEY_FULLSCREEN_TOGGLE,
    KEY_EDITOR_TOGGLE,
    KEY_RESTART,
    KEY_SHOOT
} KeyBinding;

typedef struct
{
    int bindings[MAX_KEY_BINDINGS];
} KeyBindings;

void initKeyBindings(KeyBindings* k);
int getKey(KeyBindings* k, KeyBinding key);
void inputClearKeyboard(Input* i);

bool checkMouseButton(Game* g, int button);
bool checkKey(Game* g, int key);
bool checkKeyPressed(Game* g, int key);
bool checkKeyPressedDirect(Game* g, int key);
bool checkKeyReleased(Game* g, int key);

void setMousePosition(Game* g, int x, int y);