#ifndef GUI_H
#define GUI_H

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../playground/event_queue.h"
#include "../playground/object.h"
#include <map>

namespace gui
{
    class GUI
    {
        typedef std::map<unsigned int, Object*> Objects;

    private:
        unsigned int textureBufferID;

    public:
        GUI() {}
        GUI(GLFWwindow* window, unsigned int textureBufferID);

        void renderAll(EventQueue&, const Objects&);

    private:
        void renderScene(ImGuiWindowFlags, EventQueue&);
        void renderObjectPalette(ImGuiWindowFlags, EventQueue&);
        void renderInspector(ImGuiWindowFlags, EventQueue&, const Objects&);
    };
} // namespace gui


#endif // GUI_H
