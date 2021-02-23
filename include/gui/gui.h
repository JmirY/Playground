#ifndef GUI_H
#define GUI_H

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace gui
{
    class GUI
    {
    private:
        unsigned int textureBufferID;

    public:
        GUI() {}
        GUI(GLFWwindow* window, unsigned int textureBufferID);

        void renderAll();

    private:
        void renderScene();
        void renderObjectPalette();
        void renderInspector();
    };
} // namespace gui


#endif // GUI_H
