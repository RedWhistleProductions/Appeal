#include "Named_List.h"
#include "Dynamic_Module.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <type_traits>
struct Tracked {
    static int Live;
    Tracked() { ++Live; }
    Tracked(const Tracked&) { ++Live; }
    ~Tracked() { --Live; }
};
int Tracked::Live = 0;
template<class T> void Check(const Named_List<T> &List) {
    if(!List.Nodes) { assert(!List.Start && !List.Current && List.Current_Node == 0); return; }
    auto Node = List.Start;
    for(int I = 1; I <= List.Nodes; ++I) {
        assert(Node && Node->Next_Node->Back_Node == Node && Node->Back_Node->Next_Node == Node);
        if(Node == List.Current) assert(I == List.Current_Node && Node->Name == List.Current_Name);
        Node = Node->Next_Node;
    }
    assert(Node == List.Start);
}
int main() {
    static_assert(!std::is_copy_constructible<Plugin>::value, "DLL ownership must not be copied");
    static_assert(!std::is_copy_constructible<Dynamic_Module>::value, "Module ownership must not be copied");
    for(int N = 0; N < 100; ++N) {
        { Named_List<Tracked> List; for(int I = 0; I < N; ++I) List.Add_Node(); }
        assert(Tracked::Live == 0);
    }
    Named_List<int> Empty;
    Empty.Remove(); Empty.Next(); Empty.Back(); Empty.Move_Up(); Empty.Move_Down(); Empty.Display();
    assert(!Empty.GetAddress(1) && !Empty.GetAddress("missing") && !Empty["missing"]);
    bool Threw = false; try { Empty.Out(); } catch(const std::out_of_range&) { Threw = true; }
    assert(Threw); Check(Empty);
    for(int N = 1; N <= 10; ++N) {
        Named_List<int> List;
        for(int I = 0; I < N; ++I) List.Add_Node(std::to_string(I), I);
        for(int I = 0; I < 50; ++I) { List.Move_Up(); Check(List); List.Move_Down(); Check(List); List.Next(); Check(List); }
        List.Looped = false; List.Go_To(1); List.Move_Up(); Check(List);
        List.Last(); List.Move_Down(); Check(List);
        Named_List<int> Copy(List); Check(Copy);
        *Copy.GetAddress(1) = 999;
        assert(*List.GetAddress(1) != 999);
        Copy = Copy; Check(Copy);
        Empty = List; Check(Empty);
        Named_List<int> Moved(std::move(Copy)); Check(Copy); Check(Moved);
        Copy = std::move(Moved); Check(Copy); Check(Moved);
        while(List.Nodes) { List.Go_To((List.Nodes + 1) / 2); List.Remove(); Check(List); }
        List.Remove(); Check(List);
    }
    Named_List<std::unique_ptr<int>> Owner;
    Owner.Add_Node("owned", std::make_unique<int>(42));
    auto Moved = std::move(Owner);
    assert(!Owner.Nodes && **Moved.GetAddress("owned") == 42);
    std::cout << "NAMED_LIST_MEMORY_PASSED\n";
}
