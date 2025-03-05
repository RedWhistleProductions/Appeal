#pragma once

#include <string>
#include <iostream>

#include "Base_List.h"

// Template implementation must be in the header file
template <class T>
class Named_List : public Base_List
{
public:
    Named_List();
    ~Named_List();

    /*********************************************************
     *  Functions for Adding and removing Data from the List  *
     *********************************************************/
    void Add_Node();
    void Add_Node(std::string Name);
    void Add_Node(T Data);
    void Add_Node(std::string Name, T Data);
    void operator+=(T Data); // Simplified way of using Add_Node(T Data);
    void Remove();
    void Remove(std::string Name);
    /*****************************************
     *	Pop() calls Out() and then Remove()  *
     *****************************************/
    T Pop();
    T Pop(int N);

    /*****************************************
     *   Functions for navigating the list    *
     *****************************************/

    void Next();
    void Back();
    void Go_To(int N);            // Goes to the nth Node in the list
    bool Find(std::string Name); // returns true if Name is found
    void Last();
    T *operator[](int N);
    T *operator[](std::string Name);

    /********************************************
     *	Functions for moving data in the List   *
     ********************************************/
    void Display();
    void Move_Up();
    void Move_Down();
    void Swap(int a, int b);
    void Swap(std::string a, std::string b);
    /*
        Functions for Data Output
    */

    T Out();      // Return the Value of the Current Node
    T Out(int N); // Return the Value of the Nth Node

    // Returns the address of Value in the Nth Node
    T *GetAddress(int N);
    // Returns the address of Value in Named node
    T *GetAddress(std::string Name);

    struct Node
    {
        std::string Name;
        T Value;
        Node *Next_Node;
        Node *Back_Node;
    };

    Node *Start;   // The first Node for circular linking
    Node *Current; // The Current Node to be used
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
void Named_List<T>::Display()
{
    Go_To(1);
    for (int i = 0; i <= Nodes; i++)
    {
        std::cout << Current_Name << std::endl;
        Next();
    }
}

template <class T>
bool Named_List<T>::Find(std::string Name)
{
    if (Nodes < 1)
        return false;
    bool Done = false;
    if (Nodes < 2 or Current->Name == Name)
    {
        Done = true;
    }
    else
        Go_To(1);

    while (Done == false)
    {
        if (Current->Name == Name)
            Done = true;
        else if (Current_Node == Nodes)
            Done = true;
        else
            Next();
    }
    Current_Name = Current->Name;
    if (Current->Name == Name)
        return true;
    else
        return false;
}

template <class T>
void Named_List<T>::Last()
{
    Go_To(Nodes);
}

template <class T>
void Named_List<T>::Swap(int a, int b)
{
    Go_To(a);
    T A = Out();
    std::string Name_A = Current->Name;

    Go_To(b);
    T B = Out();
    std::string Name_B = Current->Name;

    Current->Value = A;
    Current->Name = Name_A;

    Go_To(a);
    Current->Value = B;
    Current->Name = Name_B;
    Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::Move_Up()
{
    Node *Temp = Current;
    // Extract the Current Node and seal the gap
    Current->Back_Node->Next_Node = Current->Next_Node;
    Current->Next_Node->Back_Node = Current->Back_Node;

    Back();

    Current->Back_Node->Next_Node = Temp;
    Temp->Back_Node = Current->Back_Node;
    Temp->Next_Node = Current;
    Current->Back_Node = Temp;
    Temp = Current;
    Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::Move_Down()
{
    Node *Temp = Current;
    // Extract the Current Node and seal the gap
    Current->Back_Node->Next_Node = Current->Next_Node;
    Current->Next_Node->Back_Node = Current->Back_Node;

    // Next is done twice here because the insert code puts temp infront of the current variable
    Next();
    Next();

    Current->Back_Node->Next_Node = Temp;
    Temp->Back_Node = Current->Back_Node;
    Temp->Next_Node = Current;
    Current->Back_Node = Temp;
    Temp = Current;
    Current_Name = Current->Name;
}

template <class T>
Named_List<T>::~Named_List()
{
    if (Nodes > 0)
    {
        // Start at the end
        if (Nodes > 1)
            Go_To(Nodes + 1);
        while (Current != Start)
        {
            Remove();
        }
        Current->Back_Node = 0;
        Current->Next_Node = 0;
        Start = 0;
        delete Current;
    }
}

template <class T>
void Named_List<T>::Add_Node()
{
    if (Nodes == 0) // if this is the first Node in the Named_List
    {
        Start = new Node;
        Start->Back_Node = Start;
        Start->Next_Node = Start;
        Current = Start;
        Nodes = 1;
        Current_Node = 1;
        Current_Name = Current->Name;
    }
    else if (Nodes >= 1)
    {
        // Ensure new Node is added to the end
        Current = Start->Back_Node;
        Current_Node = Nodes;

        // Make the Current Node point to a new Node
        Current->Next_Node = new Node;

        // Make the New node point back
        Current->Next_Node->Back_Node = Current;

        // Make the new Node the Current Node
        Current = Current->Next_Node;

        // Link the new node to the Start Node
        Current->Next_Node = Start;
        // Link the Start node back to the new Node
        Start->Back_Node = Current;
        Nodes++;
        Current_Node++;
        Current_Name = Current->Name;
    }
    else
    {
        std::cout << "Error negative number of nodes in Named List" << std::endl;
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
void Named_List<T>::Next()
{
    if (Nodes > 1) // Make sure there is a next Node
    {
        if (Looped)
        {
            // Increment the index
            Current_Node++;
            // Move to the next Node
            Current = Current->Next_Node;
            // if you moved to the Start Node make the index 1
            if (Current == Start)
                Current_Node = 1;
        }
        else
        {
            if (Current != Start->Back_Node)
            {
                // Increment the index
                Current_Node++;
                // Move to the next Node
                Current = Current->Next_Node;
            }
        }
    }
    Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::Back()
{
    if (Nodes > 1) // Make Sure there is a Node to go back to
    {
        if (Looped)
        {
            // Decremnet the index
            Current_Node--;
            // Move back one
            Current = Current->Back_Node;
            // if you move past the first Node set the index to the end
            if (Current == Start->Back_Node)
                Current_Node = Nodes;
        }
        else
        {
            if (Current != Start)
            {
                // Decrement the index
                Current_Node--;
                // Move back one
                Current = Current->Back_Node;
            }
        }
    }
    Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::operator+=(T Data)
{
    Add_Node(Data);
    Current_Name = Current->Name;
}

template <class T>
T Named_List<T>::Out()
{
    return Current->Value;
}

template <class T>
T Named_List<T>::Out(int n)
{
    Go_To(n);
    return Out();
}
template <class T>
T Named_List<T>::Pop(int n)
{
    Go_To(n);
    return Pop();
}

template <class T>
T Named_List<T>::Pop()
{
    T ReturnValue = Out();
    Remove();
    return ReturnValue;
}

template <class T>
void Named_List<T>::Remove()
{
    Node *Temporary = Current;
    if (Nodes > 1)
    {
        Current->Next_Node->Back_Node = Current->Back_Node;
        Current->Back_Node->Next_Node = Current->Next_Node;
        Nodes--;
        if (Current == Start)
        {
            Start = Start->Next_Node;
        }

        Current = Current->Next_Node;
    }
    else if (Nodes == 1)
    {
        Start = nullptr;
        Current->Back_Node = nullptr;
        Current->Next_Node = nullptr;
        Current = nullptr;
        Current_Node = 0;
        Nodes = 0;
    }

    Temporary->Back_Node = nullptr;
    Temporary->Next_Node = nullptr;
    Temporary->Name = "";
    delete Temporary;
    // Current_Name = Current->Name;
}

template <class T>
void Named_List<T>::Remove(std::string Name)
{
    if (Find(Name))
    {
        Remove();
    }
}

template <class T>
void Named_List<T>::Go_To(int N)
{
    // Speacial Cases

    // If N is to high goto the last Node
    if ((N >= Nodes) && (Nodes > 0)) //|| (N < 0 && Nodes > 0))
    {
        Current = Start->Back_Node;
        Current_Node = Nodes;
    }

    // If N is 0 or negative goto Start
    if (N <= 1 && Nodes > 0)
    {
        Current = Start;
        Current_Node = 1;
    }

    // Normal cases
    if (N < Current_Node && N > 1)
    {
        for (; N < Current_Node; Back())
            ;
    }
    if (N > Current_Node && N < Nodes)
    {
        for (; N > Current_Node; Next())
            ;
    }

    if (Nodes > 0)
        Current_Name = Current->Name;
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
    Go_To(N);
    return &Current->Value;
}

template <class T>
T *Named_List<T>::operator[](std::string Name)
{
    Find(Name);
    return &Current->Value;
}
