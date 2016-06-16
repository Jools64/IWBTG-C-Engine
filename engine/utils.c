float randomf()
{
    int random = rand();
    return (float)random / (float)RAND_MAX;
}

float randomfBetween(float min, float max)
{
    return (randomf() * (max-min)) + min;
}

char* loadText(char* filename)
{
    char* text = 0;
    long length;
    FILE* file = fopen(filename, "rb");

    if(file)
    {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);

        text = malloc(length + 1);
        if (text)
            fread(text, 1, length, file);
        text[length] = '\0';
        
        fclose(file);
        
        return text;
    }
    printf("Error: Did not find text file: \"%s\"\n", filename);
    
    return 0;
}

float inOutEase(float t)
{
    t *= 2;
    if (t < 1) return 0.5f * t * t;
    t--;
    return -0.5f * (t * (t - 2) - 1);
}

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

Vector2f v2f(float x, float y)
{
    Vector2f out = { x, y };
    return out;
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

Vector2f vector2iTof(Vector2i value)
{
    Vector2f v = { (float)value.x, (float)value.y };
    return v;
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

float vector2fDot(Vector2f a, Vector2f b)
{
    return (a.x * b.x) + (a.y * b.y);
}

Vector2f vector2fMultiply(Vector2f a, float b)
{
    Vector2f result = { a.x * b, a.y * b };
    return result;
}

Vector2f vector2fDivide(Vector2f a, float b)
{
    Vector2f result = { a.x / b, a.y / b };
    return result;
}

Vector2f vector2fLerp(Vector2f source, Vector2f destination, float t)
{
    Vector2f delta = vector2fSubtract(destination, source);
    return vector2fAdd(source, vector2fMultiply(delta, t));
}

float vector2fDistanceSquared(Vector2f a, Vector2f b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    
    return (dx * dx) + (dy * dy);
}

float vector2fDistance(Vector2f a, Vector2f b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    
    return sqrtf((dx * dx) + (dy * dy));
}

float vector2fDirection(Vector2f a, Vector2f b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    
    return atan2f(dy, dx);
}

float vector2fNormalizedDotProduct(Vector2f a, Vector2f b)
{
    a = vector2fNormalize(a);
    b = vector2fNormalize(b);
    
    return (a.x * b.x) + (a.y * b.y);
}

Vector2f speedDirectionToVector2f(float speed, float direction)
{
    Vector2f result = { cosf(degToRad(direction)) * speed, sinf(degToRad(direction)) * speed };
    return result;
}

bool rectanglefIntersectAt(float aX, float aY, Rectanglef* a,
                           float bX, float bY, Rectanglef* b)
{
    return !((aX + a->x) >= b->w + (b->x + bX) 
          || (aY + a->y) >= b->h + (b->y + bY)
          || (aX + a->x) + a->w <= (b->x + bX) 
          || (aY + a->y) + a->h <= (b->y + bY));
}

void gridSet(Grid* g, int x, int y, int value)
{
    if(x >= 0 && y >= 0 && x < g->width && y < g->height)
        g->data[x + (y * g->width)] = value;
}

int gridGet(Grid* g, int x, int y)
{
    if(x >= 0 && y >= 0 && x < g->width && y < g->height)
        return g->data[x + (y * g->width)];
    return -1;
}

void gridInit(Grid* g, int width, int height, MemoryPool* m)
{
    g->width = width;
    g->height = height;
    g->data = memoryPoolAllocate(m, sizeof(int) * width * height);
}