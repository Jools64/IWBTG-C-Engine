// TODO: Add aabb broadpass
bool checkRectangleIntersectSprite(Rectanglef* r, Vector2f* rp, Sprite* s, Vector2f* sp)
{
    // Get the sprite frame
    int tX = (s->frame * s->size.x) % s->texture->size.x,
        tY = ((s->frame * s->size.x) / s->texture->size.x) * s->size.y;
        
    int deltaX = -sp->x;
    int deltaY = -sp->y;
    
    // Compute the rotation transformation
    float xxc = cosf(s->angle);
    float yxc = - sinf(s->angle);
    float xyc = cosf(s->angle + (PI * 1.5));
    float yyc = - sinf(s->angle + (PI * 1.5));
    
    // Iterate through every pixel in the rectangle
    for(int i = rp->x + r->x; i <= rp->x + r->x + r->w; ++i)
        for(int t = rp->y + r->y; t <= rp->y + r->y + r->h; ++t)
        {
            bool collision = false;
            
            Texture* texture = s->texture;
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
            && x < s->texture->size.x && x <= sx + s->size.x
            && y < s->texture->size.y && y <= sy + s->size.y)
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