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

#endif // EVENT_H
