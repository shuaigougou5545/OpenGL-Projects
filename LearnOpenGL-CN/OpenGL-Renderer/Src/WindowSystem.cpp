#include "WindowSystem.h"
#include "stb_image_write.h"
#include <string>
#include <cstdlib>
#include "DebugFunction.h"
#include "FileParser.h"


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
    camera = info.camera;
    lastX = m_width / 2.0f;
    lastY = m_height / 2.0f;
    
    window = glfwCreateWindow(m_width, m_height, info.title, NULL, NULL);
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
    ig = std::make_shared<ImguiGenerator>(window);
    ig->Init();
}

void WindowSystem::shutdown()
{
    // imgui
    ig->DestroyWindow();
    
    // glfw
    glfwDestroyWindow(window);
    glfwTerminate();
}

void WindowSystem::tick(float delta_time)
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = delta_time;
    lastFrame = currentFrame;
    
    processInput(window);
    glfwPollEvents();
    
    // draw imgui
    DrawImguiUI();
}

void WindowSystem::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
    width *= 2;
    height *= 2;
#endif
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

void WindowSystem::DrawImguiUI()
{
    ig->InitWindow();
    
    ImGui::Begin("OpenGL");
    
    int window_width = 0, window_height = 0;
    glfwGetWindowSize(window, &window_width, &window_height);
    ImGui::Text("Camera position: %.1f, %.1f, %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
    ImGui::Text("Camera front: %.1f, %.1f, %.1f", camera.Front.x, camera.Front.y, camera.Front.z);
    ImGui::Text("WindowSize: width - %i, height - %i", window_width, window_height);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ig->io->Framerate, ig->io->Framerate);
    
    render_to_pic = ImGui::Button("Render to Picture");
    if(render_to_pic)
        renderToPicture("./output.jpg");
    
    ImGui::Checkbox("Normal Visualization", &normal_visualization);
    if(normal_visualization)
    {
        ImGui::SliderFloat("normal_length", &normal_visualization_len, 0.0, 1.0);
    }
    
    ImGui::Checkbox("Post Processing", &post_process_info.isUsePostProcessing);
    
    ImGui::End();
    
    // Post Processing
    if(post_process_info.isUsePostProcessing)
    {
        ImGui::Begin("Post Processing");
        ImGui::Checkbox("Gaussian Blur", &post_process_info.isUseGaussianBlur);
        if(post_process_info.isUseGaussianBlur)
        {
            ImGui::SliderFloat("blur_size", &post_process_info.guassian_blur_info.blur_size, 1.0, 100.0);
        }
        ImGui::End();
    }
    
    ImGui::Render();
    ig->DrawWindow();
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

void WindowSystem::renderToPicture(const char* file_name, GLint fbo)
{
    // PS: 每次循环中,窗口处理在渲染处理之后,所以这里能获取到当前帧的渲染数据
    // 输出fbo帧缓冲,fbo默认值为0,也就是默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    int channels = 4;
    GLenum format = GL_RGBA;
    int output_width = m_width, output_height = m_height;
    
#ifdef __APPLE__
    // 视网膜显示器特殊处理
    output_width *= 2;
    output_height *= 2;
#endif
    
    unsigned char* data = (unsigned char*)malloc(channels * output_width * output_height);
    glReadPixels(0, 0, output_width, output_height, format, GL_UNSIGNED_BYTE, data);

    // 上下翻转
    unsigned char* flipped_data = (unsigned char*)malloc(channels * output_width * output_height);
    for (int y = 0; y < output_height; y++) {
        memcpy(flipped_data + (output_height - 1 - y) * channels * output_width, data + y * channels * output_width, channels * output_width);
    }
    
    FileParser fp;
    std::string extension = fp.getExtension(file_name);
    
    if(extension == "bmp"){
        // bmp与OpenGL坐标系不同,bmp原点在左上角,opengl在左下角,需要对图像数据进行翻转
        stbi_write_bmp(file_name, output_width, output_height, channels, flipped_data);
    }else if(extension == "jpg"){
        // 最后一个参数:(1~100),数字越大质量越高
        stbi_write_jpg(file_name, output_width, output_height, channels, flipped_data, 100);
    }
    
    
#ifdef __APPLE__
    // 在Mac OS下打开文件
    std::string command = "open " + std::string(file_name);
    int result = system(command.c_str()); (void)result;
#endif
    
    free(data);
    free(flipped_data);
}
