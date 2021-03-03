#ifndef EVENT_H
#define EVENT_H

#include "geometry.h"

class Event
{
public:
    Event() {}
    virtual ~Event() {}
};

class ObjectAddedEvent : public Event
{
public:
    Geometry geometry;
public:
    ObjectAddedEvent() {}
    ObjectAddedEvent(Geometry _geometry)
        : geometry(_geometry) {}
    ~ObjectAddedEvent() {}
};

class ObjectSelectedEvent : public Event
{
public:
    unsigned int id;
    bool isCtrlPressed;
public:
    ObjectSelectedEvent() {}
    ObjectSelectedEvent(unsigned int _id, bool ctrl)
        : id(_id), isCtrlPressed(ctrl) {}
    ~ObjectSelectedEvent() {}
};

class ObjectRemovedEvent : public Event
{
public:
    ObjectRemovedEvent() {}
    ~ObjectRemovedEvent() {}
};

class SimulationStatusChangedEvent : public Event
{
public:
    SimulationStatusChangedEvent() {}
    ~SimulationStatusChangedEvent() {}
};

class ObjectPositionChangedEvent : public Event
{
public:
    unsigned int id;
    float position[3];
public:
    ObjectPositionChangedEvent() {}
    ObjectPositionChangedEvent(unsigned int _id, float (&_position)[3])
    {
        id = _id;
        position[0] = _position[0];
        position[1] = _position[1];
        position[2] = _position[2];
    }
    ~ObjectPositionChangedEvent() {}
};

#endif // EVENT_H
