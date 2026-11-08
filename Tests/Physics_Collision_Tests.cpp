#include "../Resources/Modules/Appeal_Module_Physics/Appeal_Box2D_Physics/src/Main.cpp"
#include <iostream>
void Check(bool b, const char *s) {
  if (!b)
    throw std::runtime_error(s);
}
float N(std::string n, std::string k) { return std::stof(Get(n, k)); }
bool Has(std::string n, std::string kind, std::string other) {
  for (int i = 0; i < std::stoi(Get(n, "Event_Count")); ++i)
    if (Event(n, i, "Type") == kind && Event(n, i, "Other") == other)
      return true;
  return false;
}
void Box(std::string n, float x, float y, float w, float h,
         bool sensor = false) {
  Attach(n);
  Set_Body_Type(n, "Static");
  Set(n, "Sensor", sensor ? "1" : "0");
  Set_Box(n, w, h);
  Set_Position(n, x, y, 0);
}
int main() {
  try {
    Constructor();
    Set_Units_Per_Meter(64);
    Set_Gravity(0, -1800, 0);
    Box("floor", 0, -32, 2000, 64);
    Attach("player");
    Set_Capsule("player", 20, 100);
    Set_Position("player", 0, 300, 0);
    bool hit = false;
    for (int i = 0; i < 120; ++i) {
      Step(1.f / 60);
      hit |= Has("player", "Hit", "floor");
    }
    Check(hit, "fall hit event");
    Check(Get("player", "Grounded") == "1", "ground contact");
    Check(std::abs(N("player", "Position_Y") - 50) < 2, "floor response");
    Set_Velocity("player", 0, 600, 0);
    Step(1.f / 60);
    Check(N("player", "Position_Y") > 55, "jump");
    for (int i = 0; i < 90; ++i)
      Step(1.f / 60);
    Check(Get("player", "Grounded") == "1", "land");
    Box("wall", 200, 120, 30, 300);
    Set("player", "Friction", "0");
    for (int i = 0; i < 120; ++i) {
      Set_Velocity("player", 180, N("player", "Velocity_Y"), 0);
      Step(1.f / 60);
    }
    Check(N("player", "Position_X") < 167, "wall blocks walking");
    Box("door", 80, 75, 50, 150, true);
    Set("door", "Tag", "Door");
    Set_Position("player", 0, 50, 0);
    bool enter = false, exit = false, stay = false;
    for (int i = 0; i < 90; ++i) {
      Set_Velocity("player", 120, 0, 0);
      Step(1.f / 60);
      enter |= Has("player", "Begin", "door");
      exit |= Has("player", "End", "door");
      for (int j = 0; j < std::stoi(Get("player", "Contact_Count")); ++j)
        if (Contact("player", j, "Other") == "door") {
          stay = true;
          Check(Contact("player", j, "Tag") == "Door", "trigger tag");
        }
    }
    Check(enter && exit && stay, "sensor enter stay exit");
    Set("player", "Mask", "1");
    Set("wall", "Category", "2");
    Set_Position("player", 150, 50, 0);
    for (int i = 0; i < 60; ++i) {
      Set_Velocity("player", 180, 0, 0);
      Step(1.f / 60);
    }
    Check(N("player", "Position_X") > 250, "collision filters");
    Box("water", N("player", "Position_X"), 50, 100, 100, true);
    Step(1.f / 60);
    Detach("water");
    Step(1.f / 60);
    Check(Has("player", "End", "water"), "removed sensor end event");
    bool invalid = false;
    try {
      Set_Box("player", -1, 10);
    } catch (...) {
      invalid = true;
    }
    Check(invalid, "invalid size");
    for (int i = 0; i < 100; ++i) {
      Attach("temporary");
      Set_Circle("temporary", 10);
      Detach("temporary");
      Step(1.f / 60);
    }
    Destructor();
    Constructor();
    Destructor();
    std::cout << "Physics collision tests passed\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    Destructor();
    return 1;
  }
}
