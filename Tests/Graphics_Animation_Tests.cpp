#include "../Resources/Modules/Appeal_Module_Graphics/src/Animations.h"
#include <cstdlib>
#include <iostream>
#define REQUIRE(condition) do { if(!(condition)) { std::cerr << "Failed: " #condition << " at line " << __LINE__ << std::endl; std::exit(1); } } while(false)
using namespace Bananas;
static void Tick(Animations& Player, double Time, double FPS, unsigned Cycle) {
    Player.Set_Render_Context(Time, FPS, Cycle);
    Player.Paste(0, 0, 0);
}
int main() {
    Animations_2D Adam, Eve;
    REQUIRE(Adam.Load("Standing", "sheet", 512, 320, 64, 64, 0, 8));
    REQUIRE(Adam.Load("Walking", "sheet", 512, 320, 64, 64, 8, 8));
    REQUIRE(Adam.Current_Name() == "Standing");
    REQUIRE(!Adam.Load("Invalid", "sheet", 512, 320, 0, 64, 0, 8));
    REQUIRE(!Adam.Load("Invalid", "sheet", 512, 320, 64, 64, 39, 2));
    REQUIRE(!Adam.Load("Invalid", "sheet", 512, 320, 63, 64, 0, 8));
    REQUIRE(!Adam.Select("Missing") && Adam.Current_Name() == "Standing");
    REQUIRE(Eve.Load("Standing", "sheet", 512, 320, 64, 64, 0, 8));
    REQUIRE(Adam.Set_FPS("Standing", 10));
    REQUIRE(!Adam.Set_FPS("Standing", -1));
    Tick(Adam, 0, 60, 1);
    Tick(Eve, 0, 60, 1);
    Tick(Adam, .1, 60, 2);
    Tick(Eve, .1, 60, 2);
    REQUIRE(Adam.Frame_Index() == 1 && Eve.Frame_Index() == 6);
    Tick(Adam, .2, 60, 2); // same cycle: never double-advance
    REQUIRE(Adam.Frame_Index() == 1);
    Tick(Adam, 1, 60, 3); // catch up across a wrap
    REQUIRE(Adam.Frame_Index() == 2);
    REQUIRE(Adam.Select("Standing") && Adam.Frame_Index() == 2);
    REQUIRE(Adam.Select("Walking"));
    Tick(Adam, 2, 60, 4);
    auto Frame = Adam.Paste(1, 2, 3);
    REQUIRE(Frame->Y == 64 && Frame->X == 0);
    Tick(Adam, 2.1, 20, 5); // inherited rendering FPS changes
    REQUIRE(Adam.Frame_Index() == 2);
    Adam.Reset();
    REQUIRE(Adam.Current_Name() == "Standing" && Adam.Frame_Index() == 0);
    int Completed = 0;
    Adam.On_Complete = [&](Animations&) { ++Completed; };
    REQUIRE(Adam.Set_Mode("Standing", "Hold"));
    Tick(Adam, 3, 60, 6);
    Tick(Adam, 3.7, 60, 7);
    REQUIRE(Adam.Frame_Index() == 7 && Completed == 0);
    Tick(Adam, 3.8, 60, 8);
    Tick(Adam, 4.8, 60, 9);
    REQUIRE(Adam.Frame_Index() == 7 && Completed == 1);
    Adam.Set_Mode("Walking", "Reset");
    Adam.Set_FPS("Walking", 10);
    Adam.Select("Walking");
    Tick(Adam, 5, 60, 10);
    Tick(Adam, 5.8, 60, 11);
    REQUIRE(Adam.Current_Name() == "Standing" && Adam.Frame_Index() == 0);
    REQUIRE(Completed == 2);
    // Replacing a clip preserves list order and resets only that active clip.
    REQUIRE(Adam.Load("Walking", "sheet2", 128, 64, 64, 64, 0, 2));
    REQUIRE(Adam.Sequences.Nodes == 2 && Adam.Current_Name() == "Standing");
    Part Body;
    Body.Animation_Player = std::make_shared<Animations_2D>();
    Body.Move(10, 20, 30);
    Body.Rotate(45, 90, 15);
    REQUIRE(!Body.Pasted);
    Body.Paste();
    REQUIRE(Body.X == 10 && Body.Tilt == 90 && Body.Pasted);
    Body.Paste(40, 50, 60);
    REQUIRE(Body.X == 40 && Body.Y == 50 && Body.Z == 60 && Body.Turn == 45);
    std::cout << "GRAPHICS_ANIMATION_UNIT_PASSED\n";
}
