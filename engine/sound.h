
void soundPlay(Sound* sound, float volume)
{
    Mix_PlayChannel(-1, sound->data, 0);
}

void musicPlay(Music* music, float volume, Game* g)
{
    if(g->playing)
    {
        if(g->playing == music)
        {
            Mix_ResumeMusic();
            return;
        }
        Mix_HaltMusic();
    }
    g->playing = music;
    Mix_PlayMusic(music->data, -1);
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