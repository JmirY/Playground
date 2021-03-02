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
    Object() {}
    virtual ~Object() {};

    /* 구의 반지름 또는 직육면체의 half-size 를 설정한다 */
    virtual void setGeometricData(double, ...) = 0;

    /* 도형의 속성값에 따라 강체, 충돌체, Shape 의 데이터를 갱신한다 */
    virtual void updateDerivedData() = 0;
};

class SphereObject : public Object
{
protected:
    float radius;

public:
    SphereObject() : radius(1.0f) {}

    void setGeometricData(double, ...);
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

    void setGeometricData(double, ...);
    void updateDerivedData();
};

#endif // OBJECT_H