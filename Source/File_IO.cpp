#include "File_IO.h"

File_IO::File_IO()
{
	Append = false;
	Mode = "Input";
	Is_Open = false;
}

File_IO::File_IO(std::string file)
{
	Append = false;
	Mode = "Input";
	Is_Open = false;

	Open(file);
}

void File_IO::No_Spaces()
{
	std::replace(Path.begin(), Path.end(), ' ', '_');
	std::replace(File.begin(), File.end(), ' ', '_');
}

bool File_IO::Open()
{
	std::string FullPath = Path + File;
	if(Mode == "Input")
    {
		iFile.open(FullPath.c_str());
		Is_Open = iFile.is_open();
	}
	else{
		if(Append)
        {
			oFile.open(FullPath.c_str(), std::ofstream::out | std::ofstream::app);
			Is_Open = oFile.is_open();
		}
		else
        {
			oFile.open(FullPath.c_str(), std::ofstream::out);
			Is_Open = oFile.is_open();
		}
	}

	if(Is_Open)
    {
		if(DEBUG) std::cout << FullPath << " Is Open for " << Mode << std::endl;
	}
	else
    {
		if(DEBUG) std::cout << FullPath << " did not Open\n";
	}
	return Is_Open;
}

bool File_IO::Open(std::string file)
{
	File = file;
	return Open();
}

void File_IO::Close()
{
	if(Mode == "Input")iFile.close();
	else oFile.close();
	Is_Open = false;
	if(DEBUG) std::cout << File << " is Closed" << std::endl;
}

void File_IO::Read(std::string &Str)
{
	Mode = "Input";
	if(not Is_Open)Open();

	std::string Input;
	std::string Line_With_Spaces;
	bool GoodLine;
		do{
			iFile >> Input;
			if(Input.at(0) == '\n' or Input.at(0) == '/' or Input.at(0) == '#')
            {
				GoodLine = false;
				getline(iFile, Input);
			}
			else if(Input.at(0) == '"')
            {
				Line_With_Spaces = Input;
				while(Line_With_Spaces.at(Line_With_Spaces.length()-1) != '"')
                {
					iFile >> Input;
					Line_With_Spaces += " " + Input;
				}
				//Line_With_Spaces += Input;
				//Remove the "s
				Line_With_Spaces.erase(std::remove(Line_With_Spaces.begin(), Line_With_Spaces.end(), '"'));
				Line_With_Spaces.erase(std::remove(Line_With_Spaces.begin(), Line_With_Spaces.end(), '"'));
				Input = Line_With_Spaces;
				GoodLine = true;
			}
			else GoodLine = true;

		}
        while (not GoodLine);
		Str = Input;
}

File_IO& File_IO::operator>>(std::string &Str)
{
    Read(Str);
    return *this;
}

void File_IO::Read(int &num)
{
	std::string Input;
	Read(Input);
	num = std::stoi(Input);
}

File_IO& File_IO::operator>>(int &num)
{
    Read(num);
    return *this;
}

void File_IO::Read(float &num)
{
	std::string Input;
	Read(Input);
	num = std::stof(Input);
}

File_IO& File_IO::operator>>(float &num)
{
    Read(num); return *this;
}

void File_IO::Read(double &num)
{
	std::string Input;
	Read(Input);
	num = std::stod(Input);
}

File_IO& File_IO::operator>>(double &num)
{
    Read(num);
    return *this;
}

void File_IO::Write(std::string Str)
{
	Mode = "Output";
	if(not Is_Open)Open();
	oFile << Str;
}

File_IO& File_IO::operator<<(std::string Str)
{
	if(Append){
		Append = false;
		if(Is_Open){Close();}
		Open();
	}

	Write(Str);
	return *this;
}

File_IO& File_IO::operator<(std::string Str)
{
	if(not Append){
		Append = true;
		if(Is_Open){Close();}
		Open();
	}

	Write(Str);
	return *this;
}

void File_IO::Write(int num)
{
	oFile << num;
}

File_IO& File_IO::operator<<(int num)
{
    Write(num);
    return *this;
}

void File_IO::Write(float num)
{
	oFile << num;
}

File_IO& File_IO::operator<<(float num)
{
    Write(num); return *this;
}

void File_IO::Write(double num)
{
	oFile << num;
}

File_IO& File_IO::operator<<(double num)
{
    Write(num); return *this;
}

bool File_IO::End()
{
	return iFile.eof();
}

bool File_IO::Good()
{
	return iFile.good();
}

File_IO::~File_IO()
{
	if(Is_Open)Close();
}

int File_IO::I()
{
	int i;
	Read(i);
	return i;
}

std::string File_IO::S()
{
	std::string Input;
	Read(Input);
	return Input;
}

float File_IO::F()
{
	std::string Input;
	Read(Input);
	return std::stof(Input);
}

double File_IO::D()
{
	std::string Input;
	Read(Input);
	return std::stod(Input);
}