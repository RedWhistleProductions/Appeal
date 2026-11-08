#pragma once

#include <deque>
#include <map>
#include <string>
#include <vector>

// Per-interpreter, single-threaded support for script-owned game loops.
class Runtime_Events
{
public:
    struct Event { std::string Name, Value; };
    struct Timer { double Due; bool Real; Event Message; };
    std::map<std::string, std::vector<std::string>> Listeners;
    std::map<std::string, Timer> Timers;
    std::deque<Event> Pending;
    double Game_Time = 0, Real_Time = 0;
    bool Dispatching = false;
    size_t Generation = 0;

    void Reset();
    void Emit(const std::string &Name, const std::string &Value);
    void After(const std::string &Name, double Delay, bool Real,
               const std::string &Event_Name, const std::string &Value);
    void Update(double Game, double Real);
};
