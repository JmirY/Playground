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

    ObjectAddedEvent(Geometry _geometry)
        : geometry(_geometry) {}
};

class ObjectSelectedEvent : public Event
{
public:
    unsigned int id;
    bool isCtrlPressed;

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

    ObjectMassChangedEvent(unsigned int _id, float _value)
        : id(_id), value(_value) {}
};

class LeftMouseDraggedOnSceneEvent : public Event
{
public:
    float xOffset, yOffset;

    LeftMouseDraggedOnSceneEvent(float _xOffset, float _yOffset)
        : xOffset(_xOffset), yOffset(_yOffset) {}
};

class RightMouseDraggedOnSceneEvent : public Event
{
public:
    float curX, curY;
    float prevX, prevY;

    RightMouseDraggedOnSceneEvent(float _curX, float _curY, float _prevX, float _prevY)
        : curX(_curX), curY(_curY), prevX(_prevX), prevY(_prevY) {}
};

class MouseWheelOnSceneEvent : public Event
{
public:
    float value;

    MouseWheelOnSceneEvent(float _value)
        : value(_value) {}
};

class LeftMouseClickedOnSceneEvent : public Event
{
public:
    float screenX, screenY;
    bool isCtrlPressed;

    LeftMouseClickedOnSceneEvent(float _x, float _y, bool _isCtrlPressed)
        : screenX(_x), screenY(_y), isCtrlPressed(_isCtrlPressed) {}
};

class ObjectPositionFixedEvent : public Event
{
public:
    unsigned int id;
    bool shouldBeFixed;

    ObjectPositionFixedEvent(unsigned int _id, bool _shouldBeFixed)
        : id(_id), shouldBeFixed(_shouldBeFixed) {}
};

class RenderContactInfoFlagChangedEvent : public Event
{
public:
    bool flag;

    RenderContactInfoFlagChangedEvent(bool _flag)
        : flag(_flag) {}
};

class AllObjectRemovedEvent : public Event {};

class GroundRestitutionChangedEvent : public Event
{
public:
    float value;

    GroundRestitutionChangedEvent(float _value)
        : value(_value) {}
};

class ObjectRestitutionChangedEvent : public Event
{
public:
    float value;

    ObjectRestitutionChangedEvent(float _value)
        : value(_value) {}
};

class GravityChangedEvent : public Event
{
public:
    float value;

    GravityChangedEvent(float _value)
        : value(_value) {}
};

class ObjectRotatedEvent : public Event
{
public:
    unsigned int id;
    float axisX;
    float axisY;
    float axisZ;
    float degree;

    ObjectRotatedEvent(unsigned int _id, float x, float y, float z, float _degree)
        : id(_id), axisX(x), axisY(y), axisZ(z), degree(_degree) {}
};

class OrientationResetEvent : public Event
{
public:
    unsigned int id;

    OrientationResetEvent(unsigned int _id)
        : id(_id) {}
};

#endif // EVENT_H
