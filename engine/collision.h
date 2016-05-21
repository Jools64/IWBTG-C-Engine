/*
    unsigned char sampleTextureMask(Texture* texture, float x, float y, int sx, int sy, int sw, int sh,
        float originX, float originY, float scaleX, float scaleY, float rotation)
    {
        // Move out of per pixel
        float ox = sx + originX;
        float oy = sy + originY;
        
        float xxc = cosf(rotation); // 1
        float yxc = - sinf(rotation); // 0
        float xyc = cosf(rotation + (PI * 1.5));
        float yyc = - sinf(rotation + (PI * 1.5));
        // end of TODO
        
        x += sx;
        y += sy;
        
        float ix = (x) - ox;
        float iy = (y) - oy;
        
        x = (((xxc * ix) + (xyc * iy))/scaleX) + ox;
        y = (((yxc * ix) + (yyc * iy))/scaleY) + oy;
        
        if(x >= 0 && x >= sx 
        && y >= 0 && y >= sy
        && x < texture->size.x && x < sx + sw
        && y < texture->size.y && y < sy + sh)
            return ((unsigned char*)texture->surface->pixels)[(((int)x + ((int)y * texture->size.x))*4)+3] > 0;
        return false;
    }

    bool checkRectangleIntersectSprite(Iwbtg* iw, Rectanglef* r, Vector2f* rp, Sprite* s, Vector2f* sp)
    {
        int tX = (s->frame * s->size.x) % s->texture->size.x,
            tY = ((s->frame * s->size.x) / s->texture->size.x) * s->size.y;
            
        int deltaX = -sp->x;//rp->x - sp->x;
        int deltaY = -sp->y;//rp->y - sp->y;
        
        for(int i = rp->x + r->x; i < rp->x + r->x + r->w; ++i)
            for(int t = rp->y + r->y; t < rp->y + r->y + r->h; ++t)
            {
                
                if(sampleTextureMask(s->texture, i + deltaX, t + deltaY, tX, tY, s->size.x, s->size.y,
                    s->origin.x, s->origin.y, s->scale.x, s->scale.y, s->angle))
                    return true;
            }
            
        return false;
    }
*/

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
    for(int i = rp->x + r->x; i < rp->x + r->x + r->w; ++i)
        for(int t = rp->y + r->y; t < rp->y + r->y + r->h; ++t)
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