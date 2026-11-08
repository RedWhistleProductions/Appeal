#include "Runtime_Events.h"
#include <cmath>
#include <stdexcept>

namespace
{
    // Prevent accidental unbounded growth. Dispatch limits separately bound work.
    constexpr size_t Capacity = 65536;
}

void Runtime_Events::Reset()
{
    Pending.clear();
    Timers.clear();
    Game_Time = Real_Time = 0;
    ++Generation;
    // Bindings survive a restart; Dispatching remains true inside a handler.
}

void Runtime_Events::Emit(const std::string &Name, const std::string &Value)
{
    if(Name.empty()) throw std::runtime_error("Events requires a nonempty event name");
    if(Pending.size() >= Capacity) throw std::runtime_error("Events queue capacity exceeded");
    Pending.push_back({Name, Value});
}

void Runtime_Events::After(const std::string &Name, double Delay, bool Real,
                          const std::string &Event_Name, const std::string &Value)
{
    if(Name.empty() || Event_Name.empty())
        throw std::runtime_error("Events After requires nonempty timer and event names");
    const double Due = (Real ? Real_Time : Game_Time) + Delay;
    if(!std::isfinite(Delay) || Delay < 0 || !std::isfinite(Due))
        throw std::runtime_error("Events After requires a finite nonnegative delay");
    if(Timers.find(Name) == Timers.end() && Timers.size() >= Capacity)
        throw std::runtime_error("Events timer capacity exceeded");
    Timers[Name] = {Due, Real, {Event_Name, Value}};
}

void Runtime_Events::Update(double Game, double Real)
{
    if(!std::isfinite(Game) || !std::isfinite(Real) || Game < Game_Time || Real < Real_Time)
        throw std::runtime_error("Events Update clocks must be finite and nondecreasing; use Reset to rewind");
    Game_Time = Game;
    Real_Time = Real;
    // Named timer order is deterministic. Expiry only queues an event; it never
    // calls script code during collection. Timers are one-shot, replaceable by name.
    for(auto It = Timers.begin(); It != Timers.end();)
    {
        const auto &Timer = It->second;
        if(Timer.Due <= (Timer.Real ? Real_Time : Game_Time))
        {
            Emit(Timer.Message.Name, Timer.Message.Value);
            It = Timers.erase(It);
        }
        else ++It;
    }
}
