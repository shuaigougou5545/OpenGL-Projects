#include "WindowSystem.h"


namespace rd {

WindowSystem::WindowSystem()
{
    
}

WindowSystem::~WindowSystem()
{
    
}

void WindowSystem::initialize(WindowInfo info)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef  __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    m_width = info.width;
    m_height = info.height;
    lastX = m_width / 2.0f;
    lastY = m_height / 2.0f;
    
    GLFWwindow* window = glfwCreateWindow(m_width, m_height, info.title, NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启垂直同步:让程序的渲染与显示器刷新频率同步
    
    // set window callback functions
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    if(!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
        glfwTerminate();
        return;
    }
    
    // imgui
//    ig = std::make_shared<ImguiGenerator>(window);
//    ig->Init();
}

void WindowSystem::shutdown()
{
    // imgui
    ig->DestroyWindow();
    
    // glfw
    glfwDestroyWindow(window);
    glfwTerminate();
}

void WindowSystem::run()
{
    while(!glfwWindowShouldClose(window)){
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        glfwPollEvents();
        
        InitImGui();
        SetImGuiUI();
    }
}

void WindowSystem::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void WindowSystem::processInput(GLFWwindow *window)
{
    // 因为回调函数需要static,所以要封装一层,通过指针改变变量
    WindowSystem* sys = (WindowSystem*)glfwGetWindowUserPointer(window);
    if(sys){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            sys->camera.ProcessKeyboard(FORWARD, sys->deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            sys->camera.ProcessKeyboard(BACKWARD, sys->deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            sys->camera.ProcessKeyboard(LEFT, sys->deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            sys->camera.ProcessKeyboard(RIGHT, sys->deltaTime);
    }
}

void WindowSystem::InitImGui()
{
    ig->InitWindow();
}

void WindowSystem::SetImGuiUI()
{
    ImGui::Begin("OpenGL");
    
    int window_width = 0, window_height = 0;
    glfwGetWindowSize(window, &window_width, &window_height);
    ImGui::Text("Camera position: %.1f, %.1f, %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
    ImGui::Text("Camera front: %.1f, %.1f, %.1f", camera.Front.x, camera.Front.y, camera.Front.z);
    ImGui::Text("Framebuffer: width - %i, height - %i", window_width, window_height);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ig->io->Framerate, ig->io->Framerate);
    
    render_to_pic = ImGui::Button("Render to Picture");
    
    ImGui::End();
    ImGui::Render();
}

void WindowSystem::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    WindowSystem* sys = (WindowSystem*)glfwGetWindowUserPointer(window);
    if(sys){
        if(button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(action == GLFW_PRESS)
            {
                sys->leftMouseButtonPressed = true;
            }
            else if(action == GLFW_RELEASE)
            {
                sys->leftMouseButtonPressed = false;
            }
        }
        
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            sys->mouseOnImguiWindow = true;
        else
            sys->mouseOnImguiWindow = false;
    }
}

void WindowSystem::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    WindowSystem* sys = (WindowSystem*)glfwGetWindowUserPointer(window);
    if(sys){
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        
        if (sys->firstMouse)
        {
            sys->lastX = xpos;
            sys->lastY = ypos;
            sys->firstMouse = false;
        }
        
        float xoffset = xpos - sys->lastX;
        float yoffset = sys->lastY - ypos;
        
        sys->lastX = xpos;
        sys->lastY = ypos;
        
        if(sys->leftMouseButtonPressed && !sys->mouseOnImguiWindow){
            sys->camera.ProcessMouseMovement(xoffset, yoffset);
        }
    }
}

void WindowSystem::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    WindowSystem* sys = (WindowSystem*)glfwGetWindowUserPointer(window);
    if(sys){
        sys->camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

GLFWwindow* WindowSystem::getWindow()
{
    return window;
}

void WindowSystem::getWindowSize(int& width, int& height)
{
    width = m_width;
    height = m_height;
}

}
