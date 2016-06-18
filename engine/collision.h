// TODO: Add aabb broadpass
bool checkRectangleIntersectSprite(Rectanglef* r, Vector2f* rp, Sprite* s, Vector2f* sp)
{
    // Get the sprite frame
    int tX = (s->frame * s->size.x) % s->texture->size.x,
        tY = ((s->frame * s->size.x) / s->texture->size.x) * s->size.y;
        
    int deltaX = -sp->x;
    int deltaY = -sp->y;
    
    // Compute the rotation transformation
    double xxc, yxc, xyc, yyc;

    if(s->angle == 0) // For absolute precision when no rotation has taken place
    {
        xxc = 1;
        yxc = 0;
        xyc = 0;
        yyc = 1;
    }
    else
    {
        xxc = cos(s->angle);
        yxc = - sin(s->angle);
        xyc = cos(s->angle + (PI * 1.5));
        yyc = - sin(s->angle + (PI * 1.5));
    }
    
    // Iterate through every pixel in the rectangle
    for(int i = rp->x + r->x; i < rp->x + r->x + r->w; ++i)
        for(int t = rp->y + r->y; t < rp->y + r->y + r->h; ++t)
        {
            bool collision = false;
            
            float x = i + deltaX, 
                  y = t + deltaY, 
                  sx = tX, 
                  sy = tY;

            float ox = sx + s->origin.x;
            float oy = sy + s->origin.y;
            
            x += sx;
            y += sy;
            
            float ix = (x) - ox;
            float iy = (y) - oy;
            
            x = (((xxc * ix) + (xyc * iy))/s->scale.x) + ox;
            y = (((yxc * ix) + (yyc * iy))/s->scale.y) + oy;
            
            if(x >= 0 && x >= sx 
            && y >= 0 && y >= sy
            && x < s->texture->size.x && x < sx + s->size.x
            && y < s->texture->size.y && y < sy + s->size.y)
            {
                unsigned char* data = (unsigned char*) s->texture->surface->pixels;
                int index = (((int)x + ((int)y * s->texture->size.x))*4) + 3;
                collision = data[index] > 0;
            }
            
            if(collision)
                return true;
        }
        
    return false;
}

// Implementation credit to:
// http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect

char lineSegmentIntersection(Linef a, Linef b, Vector2f* intersection)
{
    Vector2f s1, s2;
    float s, t;
    
    s1.x = a.b.x - a.a.x;     
    s1.y = a.b.y - a.a.y;
    s2.x = b.b.x - b.a.x;     
    s2.y = b.b.y - b.a.y;

    s = (-s1.y * (a.a.x - b.a.x) + s1.x * (a.a.y - b.a.y))
      / (-s2.x * s1.y + s1.x * s2.y);
    t = ( s2.x * (a.a.y - b.a.y) - s2.y * (a.a.x - b.a.x))
      / (-s2.x * s1.y + s1.x * s2.y);

    if (s >= 0 && s <= 1
     && t >= 0 && t <= 1)
    {
        if (intersection != 0)
        {
            intersection->x = a.a.x + (t * s1.x);
            intersection->y = a.a.y + (t * s1.y);
        }
        return 1;
    }

    return 0;
}
