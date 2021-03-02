#include <gui/gui.h>
#include <GLFW/glfw3.h>
#include <playground/geometry.h>

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

void GUI::renderAll(EventQueue& eventQueue)
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
    renderInspector(windowFlags, eventQueue);

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

        ImVec2 buttonSize(100, 100);
        if (ImGui::Button("Sphere", buttonSize))
        {
            eventQueue.push(new ObjectAdditionEvent(SPHERE));
        }
        ImGui::SameLine();
        if (ImGui::Button("Box", buttonSize))
        {
            eventQueue.push(new ObjectAdditionEvent(BOX));
        }
        
        ImGui::EndChild();
    }
    ImGui::End();
}

void GUI::renderInspector(ImGuiWindowFlags windowFlags, EventQueue& eventQueue)
{    
    ImGui::Begin("Inspector", NULL, windowFlags);
    {
        ImGui::BeginChild("InspectorRender");
        ImGui::EndChild();
    }
    ImGui::End();
}
