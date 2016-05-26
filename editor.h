typedef enum EditorMode
{
    EditorMode_entities,
    EditorMode_controllers,
    EditorMode_scripts,
    EditorMode_count
} EditorMode;

typedef struct
{
    bool enabled;
    int mode;
    int selectedObject;
    Grid* editLayer;
    Sprite objectSprite, controllerSprite;
    Texture* objectsTexture;
    Texture* controllersTexture;
} Editor;

void editorInit(Iwbtg* iw);
void editorUpdate(Iwbtg* iw);
void editorDraw(Iwbtg* iw);