#pragma once

#include <EngineDefaults.h>

#define TEMPLATE template <typename Class> 
    
// Abstract Class for EventHandler to notify of a change
class EventHandlerBase {
public:
    virtual void execute() = 0;
};

// Event Handler Class : Handles Callback
template <typename Class>
class EventHandler : public EventHandlerBase {
    // Defining type for function pointer
    typedef void (Class::* _fptr)(void);



public:
    // Object of the Listener
    Class* object;
    // Function for callback
    _fptr function;

    EventHandler(Class* obj, _fptr func) {
        object = obj;
        function = func;
    }

    void execute() {
        (object->*function)();
    }
};
    
// Class to create a event
class Event {
    // To store all listeners of the event
    typedef std::map<int, EventHandlerBase*> EventHandlerMap;
    EventHandlerMap handlers;
    int count;
public:

    template <typename Class>
    void addListener(Class* obj, void (Class::* func)(void)) {
        handlers[count] = new EventHandler<Class>(obj, func);
        count++;
    }

    void execute() {
        for (EventHandlerMap::iterator it = handlers.begin(); it != handlers.end(); ++it) {
            it->second->execute();
        }
    }

};
    
class EventManager {
    struct EventType {
        Event* event;
        std::string name;
    };

    std::vector<EventType> _events;

    static EventManager* _Instance;

    EventManager() {};
public:
    static EventManager* Instance() {
        if (!_Instance) {
            _Instance = new EventManager();
        }
        return _Instance;
    }

    void createEvent(std::string name) {
        for (std::vector<EventType>::iterator it = _events.begin(); it != _events.end(); ++it) {
            EventType e = *it;
            if (e.name.compare(name) == 0)
                return;
        }
        EventType e;
        e.event = new Event();
        e.name = name;
        _events.push_back(e);
    }

    template <typename Class>
    bool subscribe(std::string name, Class* obj, void (Class::* func)(void)) {
        for (std::vector<EventType>::iterator it = _events.begin(); it != _events.end(); ++it) {
            EventType e = *it;
            if (e.name.compare(name) == 0) {
                e.event->addListener(obj, func);
                return true;
            }
        }
        return false;
    }

    void execute(std::string name) {
        for (std::vector<EventType>::iterator it = _events.begin(); it != _events.end(); ++it) {
            EventType e = *it;
            if (e.name.compare(name) == 0) {
                e.event->execute();
            }
        }
    }
};