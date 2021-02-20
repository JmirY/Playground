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

public:
    /* 소멸자 */
    virtual ~Object() {};

    /* 구의 반지름 또는 직육면체의 half-size 를 설정한다 */
    virtual void setData(float) = 0;
    virtual void setData(float, float, float) = 0;

    /* 오브젝트의 설정값에 따라 강체, 충돌체, Shape 의 데이터를 갱신한다 */
    virtual void updateDerivedData() = 0;
};

class SphereObject : public Object
{
protected:
    float radius;

public:
    SphereObject() : radius(1.0f) {}
    ~SphereObject() {}

    void setData(float value) { radius = value; }
    void setData(float, float, float) {}
    void updateDerivedData();
};

class BoxObject : public Object
{
protected:
    float halfX;
    float halfY;
    float halfZ;

public:
    BoxObject() : halfX(0.5f), halfY(0.5f), halfZ(0.5f) {}
    ~BoxObject() {}

    void setData(float halfSize) { halfX = halfY = halfZ = halfSize; }
    void setData(float x, float y, float z) { halfX = x; halfY = y; halfZ = z;}
    void updateDerivedData();
};

#endif // OBJECT_H
