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
    ObjectAddedEvent(Geometry _geometry)
        : geometry(_geometry) {}
};

class ObjectSelectedEvent : public Event
{
public:
    unsigned int id;
    bool isCtrlPressed;
public:
    ObjectSelectedEvent(unsigned int _id, bool ctrl)
        : id(_id), isCtrlPressed(ctrl) {}
};

class ObjectRemovedEvent : public Event {};

class SimulationStatusChangedEvent : public Event {};

class ObjectPositionChangedEvent : public Event
{
public:
    unsigned int id;
    float position[3];
public:
    ObjectPositionChangedEvent(unsigned int _id, float (&_position)[3])
    {
        id = _id;
        position[0] = _position[0];
        position[1] = _position[1];
        position[2] = _position[2];
    }
};

class ObjectVelocityChangedEvent : public Event
{
public:
    unsigned int id;
    float velocity[3];
public:
    ObjectVelocityChangedEvent(unsigned int _id, float (&_velocity)[3])
    {
        id = _id;
        velocity[0] = _velocity[0];
        velocity[1] = _velocity[1];
        velocity[2] = _velocity[2];
    }
};

class ObjectGeometricDataChangedEvent : public Event
{
public:
    unsigned int id;
    float value[3];
public:
    ObjectGeometricDataChangedEvent(unsigned int _id, float (&_value)[3])
    {
        id = _id;
        value[0] = _value[0];
        value[1] = _value[1];
        value[2] = _value[2];
    }
};

class ObjectMassChangedEvent : public Event
{
public:
    unsigned int id;
    float value;
public:
    ObjectMassChangedEvent(unsigned int _id, float _value)
        : id(_id), value(_value) {}
};

class LeftMouseDraggedOnSceneEvent : public Event
{
public:
    float xOffset, yOffset;
public:
    LeftMouseDraggedOnSceneEvent(float _xOffset, float _yOffset)
        : xOffset(_xOffset), yOffset(_yOffset) {}
};

class RightMouseDraggedOnSceneEvent : public Event
{
public:
    float curX, curY;
    float prevX, prevY;
public:
    RightMouseDraggedOnSceneEvent(float _curX, float _curY, float _prevX, float _prevY)
        : curX(_curX), curY(_curY), prevX(_prevX), prevY(_prevY) {}
};

#endif // EVENT_H
