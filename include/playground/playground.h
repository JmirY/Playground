#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "physics/simulator.h"
#include "graphics/renderer.h"
#include "gui/gui.h"
#include "object.h"
#include "event_queue.h"
#include <map>

class Playground
{
public:
    typedef std::map<unsigned int, Object*> Objects;

public:
    physics::Simulator simulator;
    graphics::Renderer renderer;
    gui::GUI userInterface;

    /* 새로운 오브젝트에 부여될 ID 값 */
    unsigned int newObjectID;

    /* 시뮬레이션 중인 오브젝트들을 저장한다 */
    Objects objects;

    EventQueue eventQueue;

public:
    Playground();
    
    /* 메인 루프를 실행한다 */
    void run();

    void addObject(Geometry);
    void removeObject(unsigned int id);

    void handleEvent(Event*);
    
private:
    void handleObjectAddedEvent(ObjectAddedEvent*);
    void handleObjectSelectedEvent(ObjectSelectedEvent*);
};

#endif // PLAYGROUND_H
