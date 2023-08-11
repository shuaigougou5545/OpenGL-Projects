#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main(int argc, const char * argv[])
{
    /// GLFW
    // glfw初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建glfw窗口，绑定opengl上下文
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL-01", NULL, NULL);
    if(!window){
        glfwTerminate();
        std::cout << "GLFW窗口创建失败" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    
    /// 注册GLFW回调函数
    // 检测窗口变动，以修改视口
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    
    /// GLAD
    // glad初始化
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        return -1;
    }
    

    /// shader
    unsigned int VS, PS;
    VS = glCreateShader(GL_VERTEX_SHADER);
    PS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(VS, 1, &vertexShaderSource, NULL);
    glShaderSource(PS, 1, &fragmentShaderSource, NULL);
    glCompileShader(VS);
    glCompileShader(PS);
    
    int success;
    char infoLog[512];
    glGetShaderiv(VS, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(VS, 512, NULL, infoLog);
        std::cout << "ERROR: VS COMPILATION FAILED: \n" << infoLog << std::endl;
    }
    glGetShaderiv(PS, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(PS, 512, NULL, infoLog);
        std::cout << "ERROR: PS COMPILATION FAILED: \n" << infoLog << std::endl;
    }
    
    unsigned int SP;
    SP = glCreateProgram();
    glAttachShader(SP, VS);
    glAttachShader(SP, PS);
    glLinkProgram(SP);
    
    glGetProgramiv(SP, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(SP, 512, NULL, infoLog);
        std::cout << "ERROR: SHADER PROGRAM LINK FAILED: \n" << infoLog << std::endl;
    }
    
    glUseProgram(SP);
    
    glDeleteShader(VS);
    glDeleteShader(PS);
    
    
    /// 数据
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };
    
    unsigned int indices[] = {
        // 绕序:顺时针
        0, 1, 3,
        1, 2, 3
    };
    
    
    /// VAO VBO EBO
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    /// OpenGL渲染主循环
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(SP);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
//        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(SP);
    
    /// GLFW
    // glfw释放资源
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
