/**********************************
* Bananas Game Engine (TM)        *
* Copyright (C) Billy Snyder 2024 *
**********************************/

#pragma once

#ifdef __MINGW32__
	///Windows
	#include "SDL2/SDL_mixer.h"
#endif

#ifdef __linux__
    /// linux
	#include "SDL2/SDL_mixer.h"
#endif

#ifdef __APPLE__
    /// OSX
#endif

#include <string>
#include <iostream>

extern  std::string Music_Path;
extern bool DEBUG;

class Music
{
public:
    bool MUSIC_ON;
    Mix_Music *Song;
    bool Loop;
    std::string Name;

    Music();
    ~Music();
    int Load(std::string);
    bool Play();
    void Stop();
};

Music::Music()
{
    //set the song pointer to null
    Song = NULL;
    Loop = false;
    MUSIC_ON = false;
}

Music::~Music()
{
    //free the memory used by the sound engine and close it
    Stop(); 
    Mix_FreeMusic( Song );
}

int Music::Load(std::string Music_File)
{
    Name = Music_File;
    Music_File = Music_Path + Name;

    if(DEBUG) std::cout << "Loading: " << Music_File << std::endl;

    if(Song != NULL)Mix_FreeMusic( Song );

    Song = Mix_LoadMUS( Music_File.c_str());
    if(Song == NULL)
    {
        MUSIC_ON = false;
        std::cout << "Music did not load" << std::endl
                  << "Path = " << Music_File << std::endl
                  << "Mix_Error = " << Mix_GetError() << std::endl;
        return 1;
    }

    MUSIC_ON = true;
    
    return 0;
}

bool Music::Play()
{
    if(MUSIC_ON)
    {
        int Result;
        if(Loop)Result = Mix_PlayMusic( Song, -1 );
        else Result = Mix_PlayMusic( Song, 0);

        if(Result < 0)
        {
            std::cout << "Music did not play" << std::endl
                      << "Track = " << Name << std::endl
                      << "Mix_Error = " << Mix_GetError() << std::endl;
            return false;
        }
        return true;
    }
    return false;
}

void Music::Stop()
{
    Mix_HaltMusic();
}
