
#ifdef __MINGW32__
	///Windows
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_mixer.h"
#endif

#ifdef __linux__
    /// linux
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_mixer.h>
#endif

#include <string>
#include "Music.h"
#include "SFX.h"
#include "Named_List.h"
#include "File_IO.h"


std::string Resources, Music_Path, SFX_Path;


int Frequency, Chanels, Chunksize;
bool Music_On, SFX_On;
bool DEBUG = true;

Named_List<Music> Sound_Track;
Named_List<SFX> SFX_List;

extern "C" void Set_Paths(std::string Path_of_Resources)
{
	Resources = Path_of_Resources;
	Music_Path = Resources + "/Audio/Music/";
    SFX_Path =  Resources + "/Audio/SFX/";
}

extern "C" void Constructor()
{
	Set_Paths("./Resources");
	Frequency = 22050;
    Chanels = 2;
    Chunksize = 4096;   
    Music_On = true;
    SFX_On = true;

	//Initialize SDL and the sound engine
	if( SDL_Init( SDL_INIT_AUDIO ) < 0 ) 
	{
		printf( "SDL_Audio could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Mix_OpenAudio( Frequency, MIX_DEFAULT_FORMAT, Chanels, Chunksize );
		Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );	
	}
}


extern "C" void Destructor()
{
    Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

extern "C" void Play()
{
	if(Music_On)
	{
		if(Sound_Track.Nodes > 0)
		{
			if(DEBUG)std::cout << "Playing: " << Sound_Track.Current->Name << std::endl;
			Sound_Track.Current->Value.Play();
		}
		else std::cout << "The playlist is empty" << std::endl;
	}
}

extern "C" void Play_Track(int Track)
{
	Sound_Track.Go_To(Track);
	Play();
}

extern "C" void Play_Song(std::string Name)
{
	Sound_Track.Find(Name);
	Play();
}


extern "C" void Load_Song(std::string Name, std::string Music_File)
{
	Sound_Track.Add_Node();
	Sound_Track.Current->Name = Music_File;
	Sound_Track.Current->Value.Load(Music_File);
	Sound_Track.Current->Name = Name;
}

extern "C" void Stop()
{
	Music_On = false;
	Sound_Track.Current->Value.Stop();
}

extern "C" void Next()
{
	Sound_Track.Next();
	Play();
}

extern "C" void Back()
{
	Sound_Track.Back();
	Play();
}

extern "C" void Load_Play_List(std::string Play_List)
{
	Play_List = Music_Path + Play_List;
	std::string Name, File_Name;
	File_IO File(Play_List);
		while(File.Good())//not File.End()
		{
			File >> Name;
			File >> File_Name;
			Load_Song(Name, File_Name);
		}
	File.Close();
	Sound_Track.Go_To(1);
}

extern "C" void Load_SFX(std::string Name, std::string SFX_File)
{
	SFX_List.Add_Node();
	SFX_List.Current->Value.Load(SFX_File);
	SFX_List.Current->Name = Name;
}

extern "C" void Play_SFX()
{
	if(SFX_On)SFX_List.Current->Value.Play();
}

extern "C" void Play_SFX_Name(std::string Name)
{
	SFX_List.Find(Name);
	Play_SFX();
}

extern "C" void Mute()
{
	SFX_On = false;
	Stop();
}

extern "C" void UnMute()
{
	SFX_On = true;
	Music_On = true;
	Play();
}