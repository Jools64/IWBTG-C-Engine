
#define MAX_LINES_PER_WALL_GEOMETRY 1024
typedef struct WallGeometry
{
    Linef lines[MAX_LINES_PER_WALL_GEOMETRY];
    int lineCount;
    
} WallGeometry;

void wallGeometryInit(WallGeometry* wg)
{
    wg->lineCount = 0;
}

void wallGeometryAddLine(WallGeometry* wg, float ax, float ay, float bx, float by)
{
    if(wg->lineCount < MAX_LINES_PER_WALL_GEOMETRY)
    {
        Linef* line = &wg->lines[wg->lineCount++];
        line->a.x = ax;
        line->a.y = ay;
        line->b.x = bx;
        line->b.y = by;
    }
    else
        printf("Error: Ran out of room for collision geometry.\n");
}

void wallGeometryCalculate(WallGeometry* wg, Level* l)
{
    for(int y = 0; y < l->ground.height; ++y)
        for(int x = 0; x < l->ground.width; ++x)
        {
            if(gridGet(&l->ground, x, y))
            {
                if(!gridGet(&l->ground, x - 1, y))
                    wallGeometryAddLine(wg, x * 32, y * 32, x * 32, (y + 1) * 32);
                if(!gridGet(&l->ground, x + 1, y))
                    wallGeometryAddLine(wg, (x + 1) * 32, y * 32, (x + 1) * 32, (y + 1) * 32);
                if(!gridGet(&l->ground, x, y - 1))
                    wallGeometryAddLine(wg, x * 32, y * 32, (x + 1) * 32, y * 32);
                if(!gridGet(&l->ground, x, y + 1))
                    wallGeometryAddLine(wg, x * 32, (y + 1) * 32, (x + 1) * 32, (y + 1) * 32);
            }
        }
}

void wallGeometryDraw(WallGeometry* wg, Game* g)
{
    for(int i = 0; i < wg->lineCount; ++i)
    {
        Linef* line = &wg->lines[i];
        lineDraw(g, line->a.x, line->a.y, line->b.x, line->b.y, 0, 1, 1, 1);
    }
}

Vector2f wallGeometryCheckIntersection(WallGeometry* wg, Linef line)
{
    Vector2f closestIntersection = line.b, result;
    float closestDistance = vector2fDistance(line.a, line.b);
    
    for(int i = 0; i < wg->lineCount; ++i)
    {
        Linef* otherLine = &wg->lines[i];
        if(lineSegmentIntersection(line, *otherLine, &result))
        {
            float dist = vector2fDistance(result, line.a);
            if(dist < closestDistance)
            {
                closestDistance = dist;
                closestIntersection = result;
            }
        }
    }
    
    return closestIntersection;
}