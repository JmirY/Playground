#include <gui/gui.h>
#include <GLFW/glfw3.h>
#include <playground/geometry.h>
#include <string>

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

void GUI::renderAll(EventQueue& eventQueue, const Objects& objects, const bool& isSimulating)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;

    renderScene(windowFlags, eventQueue);
    renderObjectPalette(windowFlags, eventQueue);
    renderInspector(windowFlags, eventQueue, objects, isSimulating);

    ImGui::ShowDemoWindow(); // test

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void GUI::renderScene(ImGuiWindowFlags windowFlags, EventQueue& eventQueue)
{
    
    ImGui::Begin("Scene", NULL, windowFlags);
    {
        ImGui::BeginChild("SceneRender");
        ImGui::Image(
            (ImTextureID)(uintptr_t) textureBufferID,
            ImGui::GetWindowSize(),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderObjectPalette(ImGuiWindowFlags windowFlags, EventQueue& eventQueue)
{    
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

void GUI::renderInspector(
    ImGuiWindowFlags windowFlags,
    EventQueue& eventQueue,
    const Objects& objects,
    const bool& isSimulating
)
{    
    ImGui::Begin("Inspector", NULL, windowFlags);

    ImGui::BeginChild("InspectorSimulationControl", ImVec2(0, 70));
    {
        /* 시뮬레이션 멈춤/재개 버튼 */
        std::string label("Pause Simulation");
        if (!isSimulating)
            label = "Resume Simulation";
        if (ImGui::Button(label.c_str()))
            eventQueue.push(new SimulationStatusChangedEvent);

        /* 오브젝트 삭제 버튼 */
        if (ImGui::Button("Remove Selected Objects"))
            eventQueue.push(new ObjectRemovedEvent);
    }
    ImGui::EndChild(); ImGui::Separator();

    ImGui::BeginChild("InspectorObjectList", ImVec2(0, 216));
    {
        /* 생성된 오브젝트들을 나열한다 */
        int i = 1;
        for (const auto& object : objects)
        {
            ImGui::PushID(i);

            if (ImGui::Selectable("Object", object.second->getIsSelected(), 0, ImVec2(50, 50)))
            {
                /* ctrl 키를 누른 채로 클릭하면 다중 선택이 가능하다 */
                if (ImGui::GetIO().KeyCtrl)
                    eventQueue.push(new ObjectSelectedEvent(object.second->getID(), true));
                else
                    eventQueue.push(new ObjectSelectedEvent(object.second->getID(), false));
            }

            if (i % 4 != 0)
                ImGui::SameLine();
            ++i;
            ImGui::PopID();
        }
    }
    ImGui::EndChild(); ImGui::Separator();

    ImGui::BeginChild("InspectorObjectAttribute");
    {

    }
    ImGui::EndChild();

    ImGui::End();
}
