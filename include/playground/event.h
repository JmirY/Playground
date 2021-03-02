#ifndef EVENT_H
#define EVENT_H

#include "geometry.h"

class Event
{
public:
    Event() {}
    virtual ~Event() {}
};

class ObjectAdditionEvent : public Event
{
public:
    Geometry geometry;
public:
    ObjectAdditionEvent() {}
    ObjectAdditionEvent(Geometry _geometry)
        : geometry(_geometry) {}
    ~ObjectAdditionEvent() {}
};

#endif // EVENT_H
