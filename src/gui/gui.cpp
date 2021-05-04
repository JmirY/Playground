#include <gui/gui.h>
#include <GLFW/glfw3.h>
#include <playground/geometry.h>
#include <string>
#include <cfloat>

using namespace gui;

GUI::GUI(GLFWwindow* window, unsigned int _textureBufferID)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    textureBufferID = _textureBufferID;

    /* imgui 전역 스타일 설정 */
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
}

void GUI::renderAll(
    EventQueue& eventQueue,
    const Objects& objects,
    const bool& isSimulating,
    const std::vector<unsigned int>& selectedObjectIDs
)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;

    renderScene(windowFlags, eventQueue, isSimulating);
    renderObjectPalette(windowFlags, eventQueue);
    renderSimulationControl(windowFlags, eventQueue, isSimulating);
    renderObjectList(windowFlags, eventQueue, objects);
    renderAttributes(windowFlags, eventQueue, objects, selectedObjectIDs, isSimulating);

    // ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void GUI::renderScene(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const bool& isSimulating
)
{
    if (!isSimulating)
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1024, 576));
    ImGui::Begin("Scene", NULL, windowFlags);
    {
        ImGui::BeginChild("SceneRender");

        /* Scene 렌더 */
        ImGui::Image(
            (ImTextureID)(uintptr_t) textureBufferID,
            ImGui::GetWindowSize(),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        const ImGuiIO& io = ImGui::GetIO();
        /* 왼쪽 마우스 드래그 */
        ImVec2 clickedPos = io.MouseClickedPos[0];
        if (isInScene(clickedPos))
        {
            if (ImGui::IsMouseDragging(0))
            {
                ImVec2 dragDelta = io.MouseDelta;
                if (dragDelta.x != 0.0f && dragDelta.y != 0.0f)
                    eventQueue.push(new LeftMouseDraggedOnSceneEvent(dragDelta.x, -dragDelta.y));
            }
            else if (ImGui::IsMouseReleased(0) && io.MouseDownDurationPrev[0] < 0.03f)
            {
                eventQueue.push(new LeftMouseClickedOnSceneEvent(clickedPos.x, clickedPos.y, io.KeyCtrl));
            }
        }

        /* 오른쪽 마우스 드래그 */
        clickedPos = io.MouseClickedPos[1];
        if (isInScene(clickedPos) && ImGui::IsMouseDragging(1))
        {
            ImVec2 dragDelta = io.MouseDelta;
            if (dragDelta.x != 0.0f && dragDelta.y != 0.0f)
            {
                eventQueue.push(new RightMouseDraggedOnSceneEvent(
                    clickedPos.x + dragDelta.x * 10.0f,
                    clickedPos.y + dragDelta.y * 10.0f,
                    clickedPos.x,
                    clickedPos.y
                ));
            }
        }
        /* 마우스 휠 스크롤 */
        ImVec2 mousePos = ImGui::GetMousePos();
        if (isInScene(mousePos))
        {
            float mouseWheel = io.MouseWheel;
            if (mouseWheel != 0.0f)
                eventQueue.push(new MouseWheelOnSceneEvent(mouseWheel));
        }

        ImGui::EndChild();
    }
    ImGui::End();

    if (!isSimulating)
        ImGui::PopStyleColor();
}

void GUI::renderObjectPalette(ImGuiWindowFlags windowFlags, EventQueue& eventQueue)
{    
    ImGui::SetNextWindowPos(ImVec2(0, 576));
    ImGui::SetNextWindowSize(ImVec2(1024, 144));
    ImGui::Begin("ObjectPalette", NULL, windowFlags);
    {
        ImGui::BeginChild("ObjectPaletteRender");

        /* 오브젝트 추가 버튼을 렌더한다 */
        ImVec2 buttonSize(100, 100);
        if (ImGui::Button("Sphere", buttonSize))
        {
            eventQueue.push(new ObjectAddedEvent(SPHERE));
        }
        ImGui::SameLine();
        if (ImGui::Button("Box", buttonSize))
        {
            eventQueue.push(new ObjectAddedEvent(BOX));
        }
        
        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderSimulationControl(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const bool& isSimulating
)
{
    ImGui::SetNextWindowPos(ImVec2(1024, 0));
    ImGui::SetNextWindowSize(ImVec2(256, 110));
    ImGui::Begin("SimulationControl", NULL, windowFlags);
    {
        ImGui::BeginChild("SimulationControlRender");

        /* 시뮬레이션 멈춤/재개 버튼 */
        std::string label("Pause Simulation");
        if (!isSimulating)
            label = "Resume Simulation";
        if (ImGui::Button(label.c_str()))
            eventQueue.push(new SimulationStatusChangedEvent);

        /* 선택된 오브젝트 삭제 버튼 */
        if (ImGui::Button("Remove Selected Objects"))
            eventQueue.push(new ObjectRemovedEvent);

        /* 고정되지 않은 오브젝트 삭제 버튼 */
        if (ImGui::Button("Remove Unfixed Objects"))
            eventQueue.push(new RemoveUnfixedObjectsEvent);

        /* 모든 오브젝트 삭제 버튼 */
        if (ImGui::Button("Remove All Objects"))
            eventQueue.push(new AllObjectRemovedEvent);

        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderObjectList(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const Objects& objects
)
{
    ImGui::SetNextWindowPos(ImVec2(1024, 110));
    ImGui::SetNextWindowSize(ImVec2(256, 206));
    ImGui::Begin("ObjectList", NULL, windowFlags);
    {
        ImGui::BeginChild("ObjectListRender");

        /* 생성된 오브젝트들을 나열한다 */
        int i = 1;
        for (const auto& object : objects)
        {
            ImGui::PushID(i);

            if (ImGui::Selectable("##Object", object.second->getIsSelected(), 0, ImVec2(50, 50)))
            {
                /* ctrl 키를 누른 채로 클릭하면 다중 선택이 가능하다 */
                if (ImGui::GetIO().KeyCtrl)
                    eventQueue.push(new ObjectSelectedEvent(object.second->getID(), true));
                else
                    eventQueue.push(new ObjectSelectedEvent(object.second->getID(), false));
            }

            /* 도형 & 색상을 표시한다 */
            glm::vec3 glmColor = object.second->getColor();
            ImU32 color = ImColor(glmColor.x, glmColor.y, glmColor.z);

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 selectableSize = ImGui::GetItemRectSize();
            ImVec2 selectableMin = ImGui::GetItemRectMin();
            if (object.second->getGeometry() == SPHERE)
            {
                ImVec2 center = selectableMin;
                center.x += selectableSize.x * 0.5f;
                center.y += selectableSize.y * 0.5f;
                drawList->AddCircleFilled(center, 20.0f, color, 100);
            }
            else if (object.second->getGeometry() == BOX)
            {
                float padding = 10.f;
                ImVec2 selectableMax = ImGui::GetItemRectMax();
                selectableMin.x += padding;
                selectableMin.y += padding;
                selectableMax.x -= padding;
                selectableMax.y -= padding;
                drawList->AddRectFilled(selectableMin, selectableMax, color);
            }

            if (i % 4 != 0)
                ImGui::SameLine();
            ++i;
            ImGui::PopID();
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderAttributes(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const Objects& objects,
    const std::vector<unsigned int>& selectedObjectIDs,
    const bool& isSimulating
)
{
    ImGui::SetNextWindowPos(ImVec2(1024, 316));
    ImGui::SetNextWindowSize(ImVec2(256, 404));

    ImGui::Begin("Attribute", NULL, windowFlags);
    {
        ImGui::BeginChild("AttributeRender");

        if (ImGui::BeginTabBar("TabBar"))
        {
            if (ImGui::BeginTabItem("Global"))
            {
                ImGui::Spacing();
                renderGlobalAttribute(eventQueue);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Object"))
            {
                ImGui::Spacing();
                renderObjectAttribute(eventQueue, objects, selectedObjectIDs, isSimulating);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderObjectAttribute(
    EventQueue& eventQueue,
    const Objects& objects,
    const std::vector<unsigned int>& selectedObjectIDs,
    const bool& isSimulating
)
{
    if (selectedObjectIDs.size() == 1)
    {
        const Object* object = objects.find(selectedObjectIDs[0])->second;
        bool isObjectFixed = object->getIsFixed();

        ImGui::Columns(4);
        if (ImGui::Checkbox("Fixed", &isObjectFixed))
            eventQueue.push(new ObjectPositionFixedEvent(selectedObjectIDs[0], isObjectFixed));
        ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
        ImGui::Text("X"); ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
        ImGui::Text("Y"); ImGui::NextColumn(); ImGui::AlignTextToFramePadding();
        ImGui::Text("Z"); ImGui::NextColumn();
        ImGui::Separator();

        float vecBuffer[3];
        /* 위치 */
        object->getPositionInArray(vecBuffer);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Position"); ImGui::NextColumn();
        if (ImGui::DragFloat("##PositionX", &vecBuffer[0], 0.05f))
            eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 0));
        ImGui::NextColumn();
        if (ImGui::DragFloat("##PositionY", &vecBuffer[1], 0.05f, 0.0f, FLT_MAX))
            eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 1));
        ImGui::NextColumn();
        if (ImGui::DragFloat("##PositionZ", &vecBuffer[2], 0.05f))
            eventQueue.push(new ObjectPositionChangedEvent(selectedObjectIDs[0], vecBuffer));
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 2));
        ImGui::NextColumn();

        /* 속도 */
        object->getVelocityInArray(vecBuffer);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Velocity"); ImGui::NextColumn();
        if (ImGui::DragFloat("##VelocityX", &vecBuffer[0], 0.1f))
        {
            if (!isSimulating)
                eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
        }
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 0));
        ImGui::NextColumn();
        if (ImGui::DragFloat("##VelocityY", &vecBuffer[1], 0.1f))
        {
            if (!isSimulating)
                eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
        }
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 1));
        ImGui::NextColumn();
        if (ImGui::DragFloat("##VelocityZ", &vecBuffer[2], 0.1f))
        {
            if (!isSimulating)
                eventQueue.push(new ObjectVelocityChangedEvent(selectedObjectIDs[0], vecBuffer));
        }
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 2));
        ImGui::NextColumn();
        ImGui::Columns(1); ImGui::Separator(); ImGui::Spacing();
        
        /* 방향 */
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Orientation"); ImGui::SameLine(0.0f, 10.0f);
        if (ImGui::Button("Reset"))
        {
            eventQueue.push(new OrientationResetEvent(selectedObjectIDs[0]));
        }
        ImVec2 buttonSize(ImGui::GetColumnWidth(), 0.0f);
        ImGui::Button("X", buttonSize);
        if (ImGui::IsItemActive())
        {
            ImVec2 dragDelta = ImGui::GetIO().MouseDelta;
            eventQueue.push(new ObjectRotatedEvent(selectedObjectIDs[0], 1.0f, 0.0f, 0.0f, dragDelta.x));
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 0));
        }
        if (ImGui::IsItemHovered())
        {
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 0));
        }
        ImGui::Button("Y", buttonSize);
        if (ImGui::IsItemActive())
        {
            ImVec2 dragDelta = ImGui::GetIO().MouseDelta;
            eventQueue.push(new ObjectRotatedEvent(selectedObjectIDs[0], 0.0f, 1.0f, 0.0f, dragDelta.x));
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 1));
        }
        if (ImGui::IsItemHovered())
        {
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 1));
        }
        ImGui::Button("Z", buttonSize);
        if (ImGui::IsItemActive())
        {
            ImVec2 dragDelta = ImGui::GetIO().MouseDelta;
            eventQueue.push(new ObjectRotatedEvent(selectedObjectIDs[0], 0.0f, 0.0f, 1.0f, dragDelta.x));
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 2));
        }
        if (ImGui::IsItemHovered())
        {
            eventQueue.push(new ShouldRenderWorldAxis(selectedObjectIDs[0], 2));
        }
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        /* 질량 */
        object->getMassInArray(vecBuffer);
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Mass");
        if (ImGui::DragFloat("##Mass", &vecBuffer[0], 0.1f, 0.1f, FLT_MAX))
            eventQueue.push(new ObjectMassChangedEvent(selectedObjectIDs[0], vecBuffer[0]));
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        /* 도형 데이터 */
        object->getGeometricDataInArray(vecBuffer);
        Geometry geometry = object->getGeometry();
        if (geometry == BOX)
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Half-size"); ImGui::SameLine(0.0f, 10.0f);
            if (ImGui::Button("Reset##halfSize"))
            {
                vecBuffer[0] = 0.5f;
                vecBuffer[1] = 0.5f;
                vecBuffer[2] = 0.5f;
                eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("X"); ImGui::SameLine();
            if (ImGui::DragFloat("##Half-X", &vecBuffer[0], 0.01f, 0.1f, FLT_MAX))
                eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::Text("Y"); ImGui::SameLine();
            if (ImGui::DragFloat("##Half-Y", &vecBuffer[1], 0.01f, 0.1f, FLT_MAX))
                eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
            ImGui::Text("Z"); ImGui::SameLine();
            if (ImGui::DragFloat("##Half-Z", &vecBuffer[2], 0.01f, 0.1f, FLT_MAX))
                eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
        }
        else if (geometry == SPHERE)
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Radius"); ImGui::SameLine(0.0f, 10.0f);
            if (ImGui::Button("Reset##radius"))
            {
                vecBuffer[0] = 1.0f;
                eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
            }
            if (ImGui::DragFloat("##Radius", &vecBuffer[0], 0.01f, 0.1f, FLT_MAX))
                eventQueue.push(new ObjectGeometricDataChangedEvent(selectedObjectIDs[0], vecBuffer));
        }
    }
}

void GUI::renderGlobalAttribute(EventQueue& eventQueue)
{
    static bool shouldRenderContactInfo = false;
    static float gravity = 9.0f;
    static float groundRestitution = 0.2f;
    static float objectRestitution = 0.3f;

    if (ImGui::Checkbox("Render contact info.", &shouldRenderContactInfo))
    {
        eventQueue.push(new RenderContactInfoFlagChangedEvent(shouldRenderContactInfo));
    } ImGui::Spacing();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Gravity");
    if (ImGui::SliderFloat("##Gravity", &gravity, 0.0f, 30.0f))
    {
        eventQueue.push(new GravityChangedEvent(gravity));
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Ground Restituion");
    if (ImGui::SliderFloat("##Ground_Restitution", &groundRestitution, 0.0f, 1.0f))
    {
        eventQueue.push(new GroundRestitutionChangedEvent(groundRestitution));
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Object Restituion");
    if (ImGui::SliderFloat("##Object_Restitution", &objectRestitution, 0.0f, 1.0f))
    {
        eventQueue.push(new ObjectRestitutionChangedEvent(objectRestitution));
    }
}

bool GUI::isInScene(ImVec2 vec)
{
    if (vec.x > 0.0f && vec.x < 1024.0f && vec.y > 0.0f && vec.y < 576.0f)
        return true;
    else
        return false;
}
