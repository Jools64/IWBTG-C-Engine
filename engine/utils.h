#define max(a,b) (a > b ? a : b)
#define min(a,b) (a < b ? a : b)
#define clamp(value,min,max) (value > max ? max : (value < min ? min : value))
#define degToRad(degrees) (degrees * (PI / 180))

typedef struct Game Game;

typedef struct
{
    int x, y;
} Vector2i;

typedef struct
{
    float x, y;
} Vector2f;

typedef struct Linef
{
    Vector2f a, b;
} Linef;

typedef struct Matrix3f
{
    union
    {
        float values[9];
        float values2d[3][3];
        struct
        {
            // GLSL order
            float _1x1, _1x2, _1x3, 
                  _2x1, _2x2, _2x3, 
                  _3x1, _3x2, _3x3;
        };
    };
} Matrix3f;

Matrix3f matrix3fIdentity()
{
    Matrix3f m = {{{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    }}};
    
    return m;
}

Vector2f vector2fApplyMatrix3f(Vector2f v, Matrix3f m)
{
    Vector2f result;
    
    result.x = (m._1x1 * v.x) + (m._2x1 * v.y) + m._3x1;
    result.y = (m._1x2 * v.x) + (m._2x2 * v.y) + m._3x2;
    
    return result;
}

typedef struct
{
    float x, y, w, h;
} Rectanglef;

typedef struct
{
    int width, height;
    int* data;
} Grid;

int loopi(int value, int min, int max);
int clampi(int value, int min, int max);
int sign(float value);
char* cloneString(Game* g, char* string);
bool checkFileExists(char* filePath);

Vector2f v2f(float x, float y);
Vector2f vector2fAdd(Vector2f a, Vector2f b);
Vector2f vector2fSubtract(Vector2f a, Vector2f b);
float vector2fMagnitude(Vector2f a);
Vector2f vector2fNormalize(Vector2f a);
Vector2f vector2fMultiply(Vector2f a, float b);
Vector2f vector2fLerp(Vector2f source, Vector2f destination, float t);
float vector2fDistanceSquared(Vector2f a, Vector2f b);
float vector2fDistance(Vector2f a, Vector2f b);
float vector2fNormalizedDotProduct(Vector2f a, Vector2f b);

bool rectanglefIntersectAt(float aX, float aY, Rectanglef* a,
                           float bX, float bY, Rectanglef* b);
                           
void gridSet(Grid* g, int x, int y, int value);
int gridGet(Grid* g, int x, int y);
void gridInit(Grid* g, int width, int height, MemoryPool* m);