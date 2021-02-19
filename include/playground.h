#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "physics/simulator.h"
#include "graphics/renderer.h"
#include "object.h"
#include <map>

class Playground
{
public:
    typedef std::map<unsigned int, Object*> Objects;

private:
    physics::Simulator simulator;
    graphics::Renderer renderer;

    /* 새로운 오브젝트에 부여될 식별자를 기억한다 */
    unsigned int idCounter;

    /* 시뮬레이션 중인 오브젝트들을 저장한다 */
    Objects objects;

public:
    /* 생성자 */
    Playground() : idCounter(0) {};
    
    /* 메인 루프를 실행한다 */
    void run();

    /* 오브젝트를 추가한다 */
    void addObject(Geometry);

    /* 오브젝트를 제거한다 */
    void removeObject(unsigned int id);
};

#endif // PLAYGROUND_H
