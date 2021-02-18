#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "physics/simulator.h"
#include "graphics/renderer.h"
#include "object.h"
#include <vector>

class Playground
{
public:
    typedef std::vector<Object*> Objects;

public:
    physics::Simulator simulator;
    graphics::Renderer renderer;

    /* 시뮬레이션 중인 물체들을 저장한다 */
    Objects objects;

public:
    /* 메인 루프를 실행한다 */
    void run();

    /* 시뮬레이션할 물체를 추가한다 */
    void addObject(Geometry);
};

#endif // PLAYGROUND_H
