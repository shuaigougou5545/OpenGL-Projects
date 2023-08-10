#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, const char * argv[])
{
    /// GLFW
    // 1.glfw初始化
    glfwInit();
    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // 2.创建glfw窗口，绑定opengl上下文
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL-01", NULL, NULL);
    if(!window){
        glfwTerminate();
        std::cout << "GLFW窗口创建失败" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    
    /// GLAD
    // 1.glad初始化
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        return -1;
    }
        
    
    /// OpenGL渲染设置
    // 1.视口，注册回调函数检测窗口变动而修改视口
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    /// OpenGL渲染主循环
    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    /// GLFW
    // 1.glfw释放资源
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

