#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>

class File_IO
{
public:
	bool DEBUG = false;
	std::string File;
	std::string Path;

	std::ofstream oFile;
	std::ifstream iFile;

	std::string Mode;
	bool Append;
	bool Is_Open;

	File_IO();
	File_IO(std::string file);
	~File_IO();

	bool Open();
	bool Open(std::string file);
	void Close();
	void No_Spaces();
	
	int I();
	std::string S();
	float F();
	double D();
		
	void Read(std::string &Str);
	File_IO& operator>>(std::string &Str);

	void Read(int &num);
	File_IO& operator>>(int &num);

	void Read(float &num);
	File_IO& operator>>(float &num);

	void Read(double &num);
	File_IO& operator>>(double &num);

	void Write(std::string Str);
	File_IO& operator<<(std::string);
	File_IO& operator<(std::string Str);

	void Write(int num);
	File_IO& operator<<(int num);

	void Write(float num);
	File_IO& operator<<(float num);

	void Write(double num);
	File_IO& operator<<(double num);

	bool Good();
	bool End();
};

