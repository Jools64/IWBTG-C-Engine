typedef enum MenuItemType
{
    MenuItemType_button,
    MenuItemType_toggle,
    MenuItemType_integer
} MenuItemType;

typedef enum MenuOrientation
{
    MenuOrientation_horizontal,
    MenuOrientation_vertical
} MenuOrientation;

typedef struct MenuItem MenuItem;
#define MAX_LABEL_LENGTH 128
typedef struct MenuItem
{
    char label[MAX_LABEL_LENGTH];
    MenuItemType type;
    Vector2f size;
    MenuItem* next, 
            * previous;
            
    void* functionData;
    void (*function)(MenuItem* mi, void* data);
    int id;
    Vector2f lastDrawPosition;
} MenuItem;

#define MAX_ELEMENTS_PER_MENU 20
typedef struct Menu
{
    MenuItem items[MAX_ELEMENTS_PER_MENU];
    int itemCount;
    MenuItem* firstItem,
            * lastItem;
    Vector2f position;
    Vector2f spacing;
    MenuItem* selected;
    MenuOrientation orientation;
    int idCounter;
    Sprite cursor;
    Vector2f cursorPosition;
    Vector2f cursor2Position;
} Menu;

typedef struct Iwbtg Iwbtg;

void menuInit(Menu* m, float x, float y, Iwbtg* iw);
MenuItem* menuAddItem(Menu* m, MenuItemType type, char* label, Iwbtg* iw);
void drawMenu(Menu* m, Iwbtg* iw);