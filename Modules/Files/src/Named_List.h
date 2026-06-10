#pragma once

#include <string>
#include <iostream>

#include "Base_List.h"

template <class T>
class Named_List : public Base_List
{
public:
	Named_List();
	~Named_List();

	void Add_Node();
	void Add_Node(std::string Name);
	void Add_Node(T Data);
	void Add_Node(std::string Name, T Data);
	void operator+=(T Data);
	void Remove();
	void Remove(std::string Name);
	T Pop();
	T Pop(int N);

	void Next();
	void Back();
	void Go_To(int N);
	bool Find(std::string Name);
	void Last();
	T *operator[](int N);
	T *operator[](std::string Name);

	void Display();
	T Out();
	T Out(int N);
	T *GetAddress(int N);
	T *GetAddress(std::string Name);

	struct Node
	{
		std::string Name;
		T Value;
		Node *Next_Node;
		Node *Back_Node;
	};

	Node *Start;
	Node *Current;
};

template <class T>
Named_List<T>::Named_List()
{
	Base_List::Looped = true;
	Start = 0;
	Current = 0;
	Base_List::Nodes = 0;
	Base_List::Current_Node = 0;
	Base_List::Current_Name = "";
}

template <class T>
Named_List<T>::~Named_List()
{
	while(Nodes > 0)
	{
		Remove();
	}
}

template <class T>
void Named_List<T>::Add_Node()
{
	if(Nodes == 0)
	{
		Start = new Node;
		Start->Back_Node = Start;
		Start->Next_Node = Start;
		Current = Start;
		Nodes = 1;
		Current_Node = 1;
		Current_Name = Current->Name;
	}
	else
	{
		Current = Start->Back_Node;
		Current_Node = Nodes;
		Current->Next_Node = new Node;
		Current->Next_Node->Back_Node = Current;
		Current = Current->Next_Node;
		Current->Next_Node = Start;
		Start->Back_Node = Current;
		Nodes++;
		Current_Node++;
		Current_Name = Current->Name;
	}
}

template <class T>
void Named_List<T>::Add_Node(T Data)
{
	Add_Node();
	Current->Value = Data;
	Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::Add_Node(std::string Name, T Data)
{
	Add_Node(Data);
	Current->Name = Name;
	Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::Add_Node(std::string Name)
{
	Add_Node();
	Current->Name = Name;
	Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::operator+=(T Data)
{
	Add_Node(Data);
}

template <class T>
void Named_List<T>::Remove()
{
	if(Nodes < 1 || Current == nullptr)
	{
		return;
	}

	Node *Temporary = Current;
	if(Nodes > 1)
	{
		Current->Next_Node->Back_Node = Current->Back_Node;
		Current->Back_Node->Next_Node = Current->Next_Node;
		if(Current == Start)
		{
			Start = Start->Next_Node;
		}
		Current = Current->Next_Node;
		Nodes--;
	}
	else
	{
		Start = nullptr;
		Current = nullptr;
		Current_Node = 0;
		Nodes = 0;
	}

	delete Temporary;
	if(Current != nullptr)
	{
		Current_Name = Current->Name;
	}
	else
	{
		Current_Name = "";
	}
}

template <class T>
void Named_List<T>::Remove(std::string Name)
{
	if(Find(Name))
	{
		Remove();
	}
}

template <class T>
T Named_List<T>::Pop()
{
	T ReturnValue = Out();
	Remove();
	return ReturnValue;
}

template <class T>
T Named_List<T>::Pop(int N)
{
	Go_To(N);
	return Pop();
}

template <class T>
void Named_List<T>::Next()
{
	if(Nodes > 1)
	{
		Current_Node++;
		Current = Current->Next_Node;
		if(Current == Start)
		{
			Current_Node = 1;
		}
	}
	if(Current != nullptr)
	{
		Current_Name = Current->Name;
	}
}

template <class T>
void Named_List<T>::Back()
{
	if(Nodes > 1)
	{
		Current_Node--;
		Current = Current->Back_Node;
		if(Current == Start->Back_Node)
		{
			Current_Node = Nodes;
		}
	}
	if(Current != nullptr)
	{
		Current_Name = Current->Name;
	}
}

template <class T>
void Named_List<T>::Go_To(int N)
{
	if(Nodes < 1)
	{
		return;
	}

	Current = Start;
	Current_Node = 1;
	while(Current_Node < N && Current_Node < Nodes)
	{
		Current = Current->Next_Node;
		Current_Node++;
	}
	Current_Name = Current->Name;
}

template <class T>
bool Named_List<T>::Find(std::string Name)
{
	if(Nodes < 1)
	{
		return false;
	}

	Go_To(1);
	for(int i = 0; i < Nodes; i++)
	{
		if(Current->Name == Name)
		{
			Current_Name = Current->Name;
			return true;
		}
		Next();
	}
	return false;
}

template <class T>
void Named_List<T>::Last()
{
	Go_To(Nodes);
}

template <class T>
void Named_List<T>::Display()
{
	Go_To(1);
	for(int i = 0; i < Nodes; i++)
	{
		std::cout << Current_Name << std::endl;
		Next();
	}
}

template <class T>
T Named_List<T>::Out()
{
	return Current->Value;
}

template <class T>
T Named_List<T>::Out(int N)
{
	Go_To(N);
	return Out();
}

template <class T>
T *Named_List<T>::GetAddress(int N)
{
	Go_To(N);
	return &Current->Value;
}

template <class T>
T *Named_List<T>::GetAddress(std::string Name)
{
	Find(Name);
	return &Current->Value;
}

template <class T>
T *Named_List<T>::operator[](int N)
{
	return GetAddress(N);
}

template <class T>
T *Named_List<T>::operator[](std::string Name)
{
	return GetAddress(Name);
}
