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

class ObjectVelocityChangedEvent : public Event
{
public:
    unsigned int id;
    float velocity[3];
public:
    ObjectVelocityChangedEvent() {}
    ObjectVelocityChangedEvent(unsigned int _id, float (&_velocity)[3])
    {
        id = _id;
        velocity[0] = _velocity[0];
        velocity[1] = _velocity[1];
        velocity[2] = _velocity[2];
    }
    ~ObjectVelocityChangedEvent() {}
};

class ObjectGeometricDataChangedEvent : public Event
{
public:
    unsigned int id;
    float value[3];
public:
    ObjectGeometricDataChangedEvent() {}
    ObjectGeometricDataChangedEvent(unsigned int _id, float (&_value)[3])
    {
        id = _id;
        value[0] = _value[0];
        value[1] = _value[1];
        value[2] = _value[2];
    }
    ~ObjectGeometricDataChangedEvent() {}
};

class ObjectMassChangedEvent : public Event
{
public:
    unsigned int id;
    float value;
public:
    ObjectMassChangedEvent() {}
    ObjectMassChangedEvent(unsigned int _id, float _value)
        : id(_id), value(_value) {}
    ~ObjectMassChangedEvent() {}
};

#endif // EVENT_H
