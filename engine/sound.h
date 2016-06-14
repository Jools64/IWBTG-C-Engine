
void soundPlay(Sound* sound, float volume)
{
    int channel = Mix_PlayChannel(-1, sound->data, 0);
    Mix_Volume(channel, (int)(volume * 128));
}

void musicPlay(Music* music, float volume, Game* g)
{
    #ifndef NO_MUSIC
        if(g->playing)
        {
            if(g->playing == music)
            {
                Mix_ResumeMusic();
                Mix_VolumeMusic((int)(volume * 128));
                return;
            }
            Mix_HaltMusic();
        }
        g->playing = music;
        Mix_PlayMusic(music->data, -1);
        
        Mix_VolumeMusic((int)(volume * 128));
        //Mix_FadeInMusic(music->data, -1,  500);
        
        
    #endif
}

void musicPlayOnce(Music* music, float volume, Game* g)
{
    #ifndef NO_MUSIC
        if(g->playing)
        {
            if(g->playing == music)
            {
                Mix_ResumeMusic();
                Mix_VolumeMusic((int)(volume * 128));
                return;
            }
            Mix_HaltMusic();
        }
        g->playing = music;
        Mix_PlayMusic(music->data, 0);
        
        Mix_VolumeMusic((int)(volume * 128));
        //Mix_FadeInMusic(music->data, 0,  500);
        
        
    #endif
}

void musicStop(Game* g)
{
    Mix_HaltMusic();
    g->playing = 0;
}

void musicPause(Game* g)
{
    if(g->playing)
    {
        Mix_PauseMusic();
    }
}

void musicResume(Game* g)
{
    if(g->playing)
    {
        Mix_ResumeMusic();
    }
}