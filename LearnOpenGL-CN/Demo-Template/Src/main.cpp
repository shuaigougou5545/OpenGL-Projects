#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "stb_image.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
std::string read_file(const char* path);


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
    
    
    float vertices[] = {
        /*
            3---0
            |   |
            2---1
         */
         1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
    };
    
    unsigned int VBO, EBO, VAO;
    
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    std::string vsCode = read_file("./Shaders/VS.vert");
    std::string fsCode = read_file("./Shaders/FS.frag");
    
    const char* vs_code = vsCode.c_str();
    const char* fs_code = fsCode.c_str();
    
    unsigned int VS, FS;
    
    VS = glCreateShader(GL_VERTEX_SHADER);
    FS = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(VS, 1, &vs_code, NULL);
    glShaderSource(FS, 1, &fs_code, NULL);
    
    glCompileShader(VS);
    glCompileShader(FS);
    
    unsigned int ID;
    
    ID = glCreateProgram();
    
    glAttachShader(ID, VS);
    glAttachShader(ID, FS);
    
    glLinkProgram(ID);
    
    glDeleteShader(VS);
    glDeleteShader(FS);
    
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(ID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
    }
    
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(ID);
    
    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::string read_file(const char* path)
{
    std::string file_string;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        file_string = stream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR [Shader Constructor]: file not successfully read" << std::endl;
    }
    
    return file_string;
}
