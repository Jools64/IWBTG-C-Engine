#ifndef UTILS_H
#define UTILS_H

#define max(a,b) a > b ? a : b
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

int loopi(int value, int min, int max);
int clampi(int value, int min, int max);
int sign(float value);
char* cloneString(Game* g, char* string);
bool checkFileExists(char* filePath);

Vector2f vector2fSubtract(Vector2f a, Vector2f b);
float vector2fMagnitude(Vector2f a);
Vector2f vector2fNormalize(Vector2f a);
Vector2f vector2fMultiply(Vector2f a, float b);

bool rectanglefIntersectAt(float aX, float aY, Rectanglef* a,
                           float bX, float bY, Rectanglef* b);

#endif // UTILS_H