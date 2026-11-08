#include "Appeal.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace
{
    double Number(const std::string &Text)
    {
        try
        {
            size_t End = 0;
            double Value = std::stod(Text, &End);
            if(End == Text.size() && std::isfinite(Value)) return Value;
        }
        catch(const std::exception &) {}
        throw std::runtime_error("Events requires a finite number: " + Text);
    }
}

void Appeal::Run_Events()
{
    std::string Command;
    *Preprocessor >> Command;
    if(Command == "Bind" || Command == "Unbind")
    {
        std::string Name, Function;
        *Preprocessor >> Name;
        *Preprocessor >> Function;
        if(Name.empty() || Function.empty()) throw std::runtime_error("Events binding names must not be empty");
        if(Command == "Bind")
        {
            if(!Appeal_Functions.Find(Function)) throw std::runtime_error("Events handler not found: " + Function);
            auto &Functions = Events.Listeners[Name];
            if(std::find(Functions.begin(), Functions.end(), Function) == Functions.end())
                Functions.push_back(Function);
        }
        else
        {
            auto It = Events.Listeners.find(Name);
            if(It != Events.Listeners.end())
            {
                auto &Functions = It->second;
                Functions.erase(std::remove(Functions.begin(), Functions.end(), Function), Functions.end());
                if(Functions.empty()) Events.Listeners.erase(It);
            }
        }
    }
    else if(Command == "Emit")
    {
        std::string Name, Value;
        *Preprocessor >> Name;
        *Preprocessor >> Value;
        Events.Emit(Name, Value);
    }
    else if(Command == "After" || Command == "After_Real")
    {
        std::string Timer, Delay, Name, Value;
        *Preprocessor >> Timer;
        *Preprocessor >> Delay;
        *Preprocessor >> Name;
        *Preprocessor >> Value;
        Events.After(Timer, Number(Delay), Command == "After_Real", Name, Value);
    }
    else if(Command == "Cancel")
    {
        std::string Name;
        *Preprocessor >> Name;
        Events.Timers.erase(Name);
    }
    else if(Command == "Update")
    {
        std::string Game, Real;
        *Preprocessor >> Game;
        *Preprocessor >> Real;
        Events.Update(Number(Game), Number(Real));
    }
    else if(Command == "Reset") Events.Reset();
    else if(Command == "Clear") { Events.Pending.clear(); ++Events.Generation; }
    else if(Command == "Pending") Preprocessor->Return_Value(std::to_string(Events.Pending.size()));
    else if(Command == "Timer_Count") Preprocessor->Return_Value(std::to_string(Events.Timers.size()));
    else if(Command == "Dispatch")
    {
        std::string Limit_Text;
        *Preprocessor >> Limit_Text;
        const double Limit = Number(Limit_Text);
        if(Limit < 1 || Limit > 65536 || std::floor(Limit) != Limit)
            throw std::runtime_error("Events Dispatch limit must be an integer from 1 to 65536");
        if(Events.Dispatching) throw std::runtime_error("Events Dispatch cannot be nested");

        // Move this batch out before callbacks. Events emitted by a handler are
        // deferred until the next dispatch, even when spare budget remains.
        std::deque<Runtime_Events::Event> Batch;
        const size_t Count = std::min(Events.Pending.size(), static_cast<size_t>(Limit));
        for(size_t I = 0; I < Count; ++I)
        {
            Batch.push_back(std::move(Events.Pending.front()));
            Events.Pending.pop_front();
        }
        auto &Variables = Data_Manager.Variables;
        const auto Name_It = Variables.find("Event_Name"), Value_It = Variables.find("Event_Value");
        const bool Had_Name = Name_It != Variables.end(), Had_Value = Value_It != Variables.end();
        const std::string Old_Name = Had_Name ? Name_It->second : "";
        const std::string Old_Value = Had_Value ? Value_It->second : "";
        Events.Dispatching = true;
        const size_t Generation = Events.Generation;
        auto Restore = [&]()
        {
            Events.Dispatching = false;
            if(Had_Name) Variables["Event_Name"] = Old_Name;
            else Variables.erase("Event_Name");
            if(Had_Value) Variables["Event_Value"] = Old_Value;
            else Variables.erase("Event_Value");
        };
        try
        {
            for(const auto &Message : Batch)
            {
                auto It = Events.Listeners.find(Message.Name);
                if(It == Events.Listeners.end()) continue;
                const auto Functions = It->second; // callbacks may rebind listeners
                for(const auto &Function : Functions)
                {
                    Variables["Event_Name"] = Message.Name;
                    Variables["Event_Value"] = Message.Value;
                    Run_Function(Function);
                    if(Done || Generation != Events.Generation) break;
                }
                if(Done || Generation != Events.Generation) break;
            }
        }
        catch(...) { Restore(); throw; }
        Restore();
    }
    else throw std::runtime_error("Unknown Events command: " + Command);
}
