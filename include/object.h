#ifndef OBJECT_H
#define OBJECT_H

#include "geometry.h"
#include "physics/body.h"
#include "graphics/opengl/glm/glm.hpp"

class Object
{
public:
    Geometry geometry;
    physics::RigidBody* body;
    physics::Collider* collider;
    glm::vec3 color;
};

class SphereObject : public Object
{
public:
    float radius;

public:
    SphereObject() : radius(1.0f) {}
};

class BoxObject : public Object
{
public:
    float halfX;
    float halfY;
    float halfZ;

public:
    BoxObject() : halfX(0.5f), halfY(0.5f), halfZ(0.5f) {}
};

#endif // OBJECT_H
