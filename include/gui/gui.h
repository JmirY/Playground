#ifndef GUI_H
#define GUI_H

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../playground/event_queue.h"
#include "../playground/object.h"
#include <map>
#include <vector>

namespace gui
{
    class GUI
    {
        typedef std::map<unsigned int, Object*> Objects;

    private:
        unsigned int textureBufferID;

    public:
        GUI(GLFWwindow* window, unsigned int textureBufferID);

        void renderAll(
            EventQueue&,
            const Objects&,
            const bool& isSimulating,
            const std::vector<unsigned int>& selectedObjectIDs
        );

    private:
        void renderScene(
            ImGuiWindowFlags,
            EventQueue&,
            const bool& isSimulating
        );
        void renderObjectPalette(ImGuiWindowFlags, EventQueue&);
        void renderSimulationControl(
            ImGuiWindowFlags,
            EventQueue&,
            const bool& isSimulating
        );
        void renderObjectList(
            ImGuiWindowFlags,
            EventQueue&,
            const Objects&
        );
        void renderObjectAttribute(
            EventQueue&,
            const Objects&,
            const std::vector<unsigned int>& selectedObjectIDs,
            const bool& isSimulating
        );
        void renderGlobalAttribute(EventQueue&);

        bool isInScene(ImVec2);
    };
} // namespace gui


#endif // GUI_H
