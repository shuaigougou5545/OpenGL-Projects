#ifndef ImguiGenerator_h
#define ImguiGenerator_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


class ImguiGenerator{
public:
    ImGuiIO* io;
    GLFWwindow* window;
    
    ImguiGenerator(GLFWwindow* w) : window(w){}
    
    void Init();
    void InitWindow();
    void DrawWindow();
};


#endif /* ImguiGenerator_h */
