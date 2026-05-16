
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

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include "Music.h"
#include "SFX.h"
#include "Named_List.h"
#include "File_IO.h"


std::string Resources, Music_Path, SFX_Path;


int Frequency, Chanels, Chunksize;
bool Music_On, SFX_On;
bool Auto_Play = false;
bool Track_Was_Playing = false;
bool DEBUG = true;
bool Audio_Ready = false;
int Music_Volume = MIX_MAX_VOLUME;

Named_List<Music> Sound_Track;
Named_List<SFX> SFX_List;

std::string Trim(std::string Text)
{
	const std::string Whitespace = " \t\r\n";
	size_t Start = Text.find_first_not_of(Whitespace);
	if(Start == std::string::npos) return "";

	size_t End = Text.find_last_not_of(Whitespace);
	return Text.substr(Start, End - Start + 1);
}

std::string Song_Name_From_File(std::string File)
{
	size_t Slash = File.find_last_of("/\\");
	if(Slash != std::string::npos)
	{
		File = File.substr(Slash + 1);
	}

	size_t Dot = File.find_last_of('.');
	if(Dot != std::string::npos)
	{
		File = File.substr(0, Dot);
	}

	return File;
}

bool Read_Playlist_Line(std::string Line, std::string &Name, std::string &File_Name)
{
	Line = Trim(Line);
	if(Line == "" || Line[0] == '#' || Line[0] == '/') return false;

	if(Line[0] == '"')
	{
		size_t End_Quote = Line.find('"', 1);
		if(End_Quote != std::string::npos)
		{
			Name = Line.substr(1, End_Quote - 1);
			File_Name = Trim(Line.substr(End_Quote + 1));
			return File_Name != "";
		}
	}

	std::istringstream Stream(Line);
	std::string First, Second;
	Stream >> First;
	Stream >> Second;

	if(First == "") return false;

	if(Second == "")
	{
		File_Name = First;
		Name = Song_Name_From_File(First);
	}
	else
	{
		Name = First;
		File_Name = Second;
	}

	return true;
}

bool Ensure_Audio()
{
	if(Audio_Ready)
	{
		return true;
	}

	if(SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		printf("SDL_Audio could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not open audio! Mix_Error: %s\n", Mix_GetError());
		return false;
	}

	Audio_Ready = true;
	Mix_VolumeMusic(Music_Volume);
	if(DEBUG)
	{
		std::cout << "SDL audio driver: " << SDL_GetCurrentAudioDriver() << std::endl;
	}
	return true;
}

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

	// Initialize SDL audio, but leave the audio device closed until playback.
	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		printf( "SDL_Audio could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
}


extern "C" void Destructor()
{
	if(Audio_Ready)
	{
		Mix_HaltMusic();
		Mix_HaltChannel(-1);
		Mix_CloseAudio();
		Audio_Ready = false;
	}

	if(SDL_WasInit(SDL_INIT_AUDIO) != 0)
	{
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}

extern "C" void Debug(bool Enable)
{
	DEBUG = Enable;
}

extern "C" void Play()
{
	Music_On = true;
	if(Music_On && Ensure_Audio())
	{
		if(Sound_Track.Nodes > 0)
		{
			if(Sound_Track.Current->Value.Play() && DEBUG)
			{
				std::cout << "Playing: " << Sound_Track.Current->Name << std::endl;
			}
			Track_Was_Playing = Mix_PlayingMusic() != 0;
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
	if(Sound_Track.Find(Name))
	{
		Play();
	}
	else
	{
		std::cout << "Song not found: " << Name << std::endl;
	}
}


extern "C" void Load_Song(std::string Name, std::string Music_File)
{
	if(!Ensure_Audio()) return;

	Sound_Track.Add_Node();
	Sound_Track.Current->Name = Music_File;
	Sound_Track.Current->Value.Load(Music_File);
	Sound_Track.Current->Name = Name;
}

extern "C" void Stop()
{
	Music_On = false;
	Track_Was_Playing = false;
	if(Sound_Track.Nodes > 0)
	{
		Sound_Track.Current->Value.Stop();
	}
	if(Audio_Ready)
	{
		Mix_HaltMusic();
		Mix_HaltChannel(-1);
		Mix_CloseAudio();
		Audio_Ready = false;
	}
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
	std::ifstream File(Play_List.c_str());

	if(!File.is_open())
	{
		std::cout << "Playlist did not open: " << Play_List << std::endl;
		return;
	}

	std::string Line;
	while(std::getline(File, Line))
	{
		if(Read_Playlist_Line(Line, Name, File_Name))
		{
			Load_Song(Name, File_Name);
		}
	}

	if(Sound_Track.Nodes > 0)
	{
		Sound_Track.Go_To(1);
	}
}

extern "C" void Load_SFX(std::string Name, std::string SFX_File)
{
	if(!Ensure_Audio()) return;

	SFX_List.Add_Node();
	SFX_List.Current->Value.Load(SFX_File);
	SFX_List.Current->Name = Name;
}

extern "C" void Play_SFX()
{
	if(SFX_On && Ensure_Audio())SFX_List.Current->Value.Play();
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

extern "C" void Set_Volume(int Volume)
{
	if(Volume < 0) Volume = 0;
	if(Volume > 128) Volume = 128;
	Music_Volume = Volume;
	if(Audio_Ready)
	{
		Mix_VolumeMusic(Music_Volume);
	}
}

extern "C" int Get_Volume()
{
	return Music_Volume;
}

extern "C" void Set_Volume_Percent(int Percent)
{
	if(Percent < 0) Percent = 0;
	if(Percent > 100) Percent = 100;
	Set_Volume((Percent * MIX_MAX_VOLUME + 50) / 100);
}

extern "C" int Get_Volume_Percent()
{
	return (Music_Volume * 100 + (MIX_MAX_VOLUME / 2)) / MIX_MAX_VOLUME;
}

extern "C" void Set_Current_Loop(int Loop)
{
	if(Sound_Track.Nodes > 0)
	{
		Sound_Track.Current->Value.Loop = Loop != 0;
		if(Music_On && Audio_Ready && Mix_PlayingMusic())
		{
			Play();
		}
	}
}

extern "C" bool Current_Looping()
{
	if(Sound_Track.Nodes > 0)
	{
		return Sound_Track.Current->Value.Loop;
	}
	return false;
}

extern "C" void Set_Auto_Play(int Auto_Play_On)
{
	Auto_Play = Auto_Play_On != 0;
}

extern "C" bool Auto_Playing()
{
	return Auto_Play;
}

extern "C" void Update()
{
	if(!Auto_Play || !Music_On || !Audio_Ready || Sound_Track.Nodes < 1)
	{
		if(Audio_Ready)
		{
			Track_Was_Playing = Mix_PlayingMusic() != 0;
		}
		return;
	}

	bool Playing = Mix_PlayingMusic() != 0;
	if(Track_Was_Playing && !Playing)
	{
		Sound_Track.Next();
		Play();
		return;
	}

	Track_Was_Playing = Playing;
}

extern "C" std::string Current_Song()
{
	if(Sound_Track.Nodes > 0)
	{
		return Sound_Track.Current->Name;
	}
	return "";
}

extern "C" bool Is_Muted()
{
	return !Music_On;
}
