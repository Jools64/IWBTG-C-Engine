#ifndef UTILS_H
#define UTILS_H

#define max(a,b) a > b ? a : b
#define min(a,b) a < b ? a : b
#define clamp(value,min,max) value > max ? max : (value < min ? min : value)

typedef struct Game Game;

typedef struct
{
    int x, y;
} Vector2i;

typedef struct
{
    float x, y;
} Vector2f;

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

#endif // UTILS_H