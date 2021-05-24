#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include "physics/simulator.h"
#include "graphics/renderer.h"
#include "gui/gui.h"
#include "object.h"
#include "event_queue.h"
#include <unordered_map>
#include <vector>

class Playground
{
public:
    typedef std::unordered_map<unsigned int, Object*> Objects;

private:
    physics::Simulator simulator;
    graphics::Renderer renderer;
    gui::GUI userInterface;

    /* 새로운 오브젝트에 부여될 ID 값 */
    unsigned int newObjectID;

    /* 시뮬레이션 중인 오브젝트들을 저장한다 */
    Objects objects;
    /* 선택된 오브젝트들의 ID 를 저장한다 */
    std::vector<unsigned int> selectedObjectIDs;

    EventQueue eventQueue;
    bool isSimulating;
    bool shouldRenderContactInfo;

public:
    Playground();
    
    /* 메인 루프를 실행한다 */
    void run();

    /* 시뮬레이션에 오브젝트를 추가하고 ID 를 반환 */
    unsigned int addObject(Geometry, float posX = 0.0f, float posY = 3.0f, float posZ = 0.0f);
    Objects::iterator removeObject(unsigned int id);

    void handleEvent(Event*);
    void handleKeyboardInput();

private:
    void clearSelectedObjectIDs();
    void loadPreset1();
    void loadPreset2();

    void handleObjectAddedEvent(ObjectAddedEvent*);
    void handleObjectSelectedEvent(ObjectSelectedEvent*);
    void handleObjectRemovedEvent(ObjectRemovedEvent*);
    void handleSimulationStatusChangedEvent(SimulationStatusChangedEvent*);
    void handleObjectPositionChangedEvent(ObjectPositionChangedEvent*);
    void handleObjectVelocityChangedEvent(ObjectVelocityChangedEvent*);
    void handleObjectGeometricDataChangedEvent(ObjectGeometricDataChangedEvent*);
    void handleObjectMassChangedEvent(ObjectMassChangedEvent*);
    void handleLeftMouseDraggedOnSceneEvent(LeftMouseDraggedOnSceneEvent*);
    void handleRightMouseDraggedOnSceneEvent(RightMouseDraggedOnSceneEvent*);
    void handleMouseWheelOnSceneEvent(MouseWheelOnSceneEvent*);
    void handleLeftMouseClickedOnSceneEvent(LeftMouseClickedOnSceneEvent*);
    void handleObjectPositionFixedEvent(ObjectPositionFixedEvent*);
    void handleRenderContactInfoFlagChangedEvent(RenderContactInfoFlagChangedEvent*);
    void handleAllObjectRemovedEvent(AllObjectRemovedEvent*);
    void handleGroundRestitutionChangedEvent(GroundRestitutionChangedEvent*);
    void handleObjectRestitutionChangedEvent(ObjectRestitutionChangedEvent*);
    void handleGravityChangedEvent(GravityChangedEvent*);
    void handleObjectRotatedEvent(ObjectRotatedEvent*);
    void handleOrientationResetEvent(OrientationResetEvent*);
    void handleShouldRenderWorldAxis(ShouldRenderWorldAxis*);
    void handleRemoveUnfixedObjectsEvent(RemoveUnfixedObjectsEvent*);
};

#endif // PLAYGROUND_H
