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
public:
    ObjectSelectedEvent() {}
    ObjectSelectedEvent(unsigned int _id)
        : id(_id) {}
    ~ObjectSelectedEvent() {}
};

#endif // EVENT_H
