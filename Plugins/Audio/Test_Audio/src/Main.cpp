/***************************************************
* Test Plugin for Appeals Audio Module (TM)        *
* Copyright (C) Billy Snyder 2025                  *
***************************************************/

#include <iostream>
extern "C" void Debug(bool Enable) { (void)Enable; }

#include <string>

std::string Resources, Music_Path, SFX_Path;


int Frequency, Chanels, Chunksize;
bool Music_On, SFX_On;
bool DEBUG = true;

extern "C" void Set_Paths(std::string Path_of_Resources)
{
	std::cout << "Set_Paths(" << Path_of_Resources << ")" << std::endl;
}

extern "C" void Constructor()
{
	std::cout << "Audio Module Test Plugin Constructor" << std::endl;
}


extern "C" void Destructor()
{
	std::cout << "Audio Module Test Plugin Destructor" << std::endl;
}

extern "C" void Play()
{
	std::cout << "Play()" << std::endl;
}

extern "C" void Play_Track(int Track)
{
	std::cout << "Play_Track(" << Track << ")" << std::endl;
}

extern "C" void Play_Song(std::string Name)
{
	std::cout << "Play_Song(" << Name << ")" << std::endl;
}


extern "C" void Load_Song(std::string Name, std::string Music_File)
{
	std::cout << "Load_Song(" << Name << ", " << Music_File << ")" << std::endl;
}

extern "C" void Stop()
{
	std::cout << "Stop()" << std::endl;
}

extern "C" void Next()
{
	std::cout << "Next()" << std::endl;
}

extern "C" void Back()
{
	std::cout << "Back()" << std::endl;
}

extern "C" void Load_Play_List(std::string Play_List)
{
	std::cout << "Load_Play_List(" << Play_List << ")" << std::endl;
}

extern "C" void Load_SFX(std::string Name, std::string SFX_File)
{
	std::cout << "Load_SFX(" << Name << ", " << SFX_File << ")" << std::endl;
}

extern "C" void Play_SFX()
{
	std::cout << "Play_SFX()" << std::endl;
}

extern "C" void Play_SFX_Name(std::string Name)
{
	std::cout << "Play_SFX_Name(" << Name << ")" << std::endl;
}

extern "C" void Mute()
{
	std::cout << "Mute()" << std::endl;
}

extern "C" void UnMute()
{
    std::cout << "UnMute()" << std::endl;
}

extern "C" void Set_Volume(int Volume)
{
    std::cout << "Set_Volume(" << Volume << ")" << std::endl;
}

extern "C" int Get_Volume()
{
    std::cout << "Get_Volume()" << std::endl;
    return 128;
}

extern "C" std::string Current_Song()
{
    std::cout << "Current_Song()" << std::endl;
    return "";
}

extern "C" bool Is_Muted()
{
    std::cout << "Is_Muted()" << std::endl;
    return false;
}

