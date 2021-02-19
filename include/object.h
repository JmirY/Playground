#ifndef OBJECT_H
#define OBJECT_H

#include "geometry.h"
#include "physics/body.h"
#include "physics/collision.h"
#include "graphics/opengl/glm/glm.hpp"
#include "graphics/shape.h"

/* 전방 선언 */
class Playground;

class Object
{
public:
    friend class Playground;

protected:
    unsigned int id;
    Geometry geometry;
    physics::RigidBody* body;
    physics::Collider* collider;
    glm::vec3 color;
    graphics::Shape* shape;
};

class SphereObject : public Object
{
protected:
    float radius;

public:
    SphereObject() : radius(1.0f) {}

    void setRadius(float value) { radius = value; }
    /* 오브젝트의 설정값에 따라 강체, 충돌체, Shape 의 데이터를 갱신한다 */
    void updateData();
};

class BoxObject : public Object
{
protected:
    float halfX;
    float halfY;
    float halfZ;

public:
    BoxObject() : halfX(0.5f), halfY(0.5f), halfZ(0.5f) {}

    void setHalfX(float x) { halfX = x; }
    void setHalfY(float y) { halfY = y; }
    void setHalfZ(float z) { halfZ = z; }
    void updateData();
};

#endif // OBJECT_H
