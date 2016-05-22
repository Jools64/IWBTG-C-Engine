int loopi(int value, int min, int max)
{
    int delta = max - min;
    int r =  (value % delta);
    if(r < 0)
        value += delta;
    value = min + r;
    return value;
}

int clampi(int value, int min, int max)
{
    return value < min ? min : value > max ? max : value;
}

int sign(float value)
{
    return value > 0 ? 1 : value < 0 ? -1 : 0;
}

char* cloneString(Game* g, char* string)
{
    char* clone = memoryPoolAllocate(&g->globalMemory, strlen(string) + 1);
    strcpy(clone, string);
    return clone;
}

bool checkFileExists(char* filePath)
{
    FILE* file;
    if((file = fopen(filePath, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}

Vector2f vector2fAdd(Vector2f a, Vector2f b)
{
    Vector2f result = { a.x + b.x, a.y + b.y };
    return result;
}

Vector2f vector2fSubtract(Vector2f a, Vector2f b)
{
    Vector2f result = { a.x - b.x, a.y - b.y };
    return result;
}

float vector2fMagnitude(Vector2f a)
{
    return sqrtf((a.x * a.x) + (a.y * a.y));
}

Vector2f vector2fNormalize(Vector2f a)
{
    float magnitude = vector2fMagnitude(a);
    if(magnitude > 0)
    {
        Vector2f result = { a.x / magnitude, a.y / magnitude };
        return result;
    }
    else
        return a;
}

Vector2f vector2fMultiply(Vector2f a, float b)
{
    Vector2f result = { a.x * b, a.y * b };
    return result;
}

Vector2f vector2fLerp(Vector2f source, Vector2f destination, float t)
{
    Vector2f delta = vector2fSubtract(destination, source);
    return vector2fAdd(source, vector2fMultiply(delta, t));
}


bool rectanglefIntersectAt(float aX, float aY, Rectanglef* a,
                           float bX, float bY, Rectanglef* b)
{
    return !((aX + a->x) > b->w + (b->x + bX) 
          || (aY + a->y) > b->h + (b->y + bY)
          || (aX + a->x) + a->w <= (b->x + bX) 
          || (aY + a->y) + a->h <= (b->y + bY));
}