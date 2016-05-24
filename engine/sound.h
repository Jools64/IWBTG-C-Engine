
void soundPlay(Sound* sound, float volume)
{
    Mix_PlayChannel(-1, sound->data, 0);
}