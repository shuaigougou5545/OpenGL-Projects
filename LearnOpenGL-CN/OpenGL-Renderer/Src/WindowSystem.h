#ifndef WindowSystem_h
#define WindowSystem_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Camera.h"
#include "ImguiGenerator.h"

#include <iostream>
#include <memory>


struct WindowInfo
{
    int width = 800;
    int height = 600;
    const char* title = "OpenGL";
    Camera camera = Camera(glm::vec3(0.0f, 5.0f, 20.0f));
};

class WindowSystem{
public:
    WindowSystem();
    ~WindowSystem();
    
    void initialize(WindowInfo info);
    void tick(float delta_time);
    void shutdown();
    
    // property
    float lastX = 0.f;
    float lastY = 0.f;
    bool firstMouse = true;
    bool leftMouseButtonPressed = false;
    bool mouseOnImguiWindow = false;
    
    // camera
    Camera camera;
    
    // time
    float deltaTime = 0.f;
    float lastFrame = 0.f;
    
    bool render_to_pic = false;
    
    void processInput(GLFWwindow *window);
    
    void InitImGui();
    void SetImGuiUI();
    
private:
    // glfw window callback function
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    
private:
    GLFWwindow* window;
    std::shared_ptr<ImguiGenerator> ig;
    int m_width = 0, m_height = 0;
    
public:
    GLFWwindow* getWindow();
    void getWindowSize(int& width, int& height);
};


#endif /* WindowSystem_h */
