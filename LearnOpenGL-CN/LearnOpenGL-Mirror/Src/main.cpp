#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <thread>
#include <vector>

#include "DebugFunction.h"
#include "Camera.h"
#include "ShaderConstructor.h"
#include "LoadTexture.h"
#include "ImguiGenerator.h"
#include "Model.h"

#include "Material.h"
#include "Light.h"



const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 3.0f, 10.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool leftMouseButtonPressed = false;
bool mouseOnImguiWindow = false;

float deltaTime = 0.f;
float lastFrame = 0.f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef  __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启垂直同步:让程序的渲染与显示器刷新频率同步
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    if(!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
        glfwTerminate();
        return -1;
    }
    
    
    //
    // imgui
    //
    ImguiGenerator ig(window);
    ig.Init();
    glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    
    //
    // vertex input:
    //
    Model box("./Models/box.txt");
    Model skull("./Models/skull.txt");
    
    //
    // VAO VBO EBO
    //
    unsigned int VAO[2], VBO[2], EBO[2];
    
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);
    
    // box
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    
    glBufferData(GL_ARRAY_BUFFER, box.vertices_vbo.size() * sizeof(float), box.vertices_vbo.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, box.indices.size() * sizeof(unsigned int), box.indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // skull
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    
    glBufferData(GL_ARRAY_BUFFER, skull.vertices_vbo.size() * sizeof(float), skull.vertices_vbo.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, skull.indices.size() * sizeof(unsigned int), skull.indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    //
    // shader
    //
    ShaderConstructor sc("./Shaders/VS.vert", "./Shaders/FS.frag");
    sc.use();
    
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glm::mat4 model = glm::mat4(1.0), view = glm::mat4(1.0), projection = glm::mat4(1.0);
        Material mat;
        
        processInput(window);
        glfwPollEvents();
        
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        ig.InitWindow();
        ImGui::Begin("OpenGL");
        
        int window_width = 0, window_height = 0;
        glfwGetWindowSize(window, &window_width, &window_height);
        ImGui::Text("Camera position: %.1f, %.1f, %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Camera front: %.1f, %.1f, %.1f", camera.Front.x, camera.Front.y, camera.Front.z);
        ImGui::Text("Framebuffer: width - %i, height - %i", window_width, window_height);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ig.io->Framerate, ig.io->Framerate);
        
        ImGui::End();
        ImGui::Render();
        
        
        //
        // Shared Uniform: Lights, view, projection
        //
        glm::vec4 gAmbientLight = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
        sc.setVec4("gAmbientLight", glm::value_ptr(gAmbientLight));
        
        const int MaxLights = 16;
        Light lights[MaxLights];
        lights[0].Strength = glm::vec3(0.6f, 0.6f, 0.6f);
        lights[0].Direction = glm::vec3(0.57735f, -0.57735f, 0.57735f);
        lights[1].Strength = glm::vec3(0.3f, 0.3f, 0.3f);
        lights[1].Direction = glm::vec3(-0.57735f, -0.57735f, 0.57735f);
        lights[2].Strength = glm::vec3(0.15f, 0.15f, 0.15f);
        lights[2].Direction = glm::vec3(0.0f, -0.707f, -0.707f);
        sc.setVec3("gLights[0].Strength", glm::value_ptr(lights[0].Strength));
        sc.setVec3("gLights[0].Direction", glm::value_ptr(lights[0].Direction));
        sc.setVec3("gLights[1].Strength", glm::value_ptr(lights[1].Strength));
        sc.setVec3("gLights[1].Direction", glm::value_ptr(lights[1].Direction));
        sc.setVec3("gLights[2].Strength", glm::value_ptr(lights[2].Strength));
        sc.setVec3("gLights[2].Direction", glm::value_ptr(lights[2].Direction));
        sc.setVec3("gViewPos", glm::value_ptr(camera.Position));
        
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
        sc.setMat4("view", glm::value_ptr(view));
        sc.setMat4("projection", glm::value_ptr(projection));
        
        
        //
        // Shared Status: CULL_FACE
        //
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        //
        // 1.box
        //
        glEnable(GL_DEPTH_TEST);
        glm::vec3 box_trans = glm::vec3(cos(glfwGetTime()/2.0)*5.0, sin(glfwGetTime()/2.0)*3.0, 5.f);
        float angle = 180.0f + glfwGetTime() * 20.0f;
        glm::vec3 box_scale = glm::vec3(0.5f);
        model = glm::mat4(1.0);
        model = glm::translate(model, box_trans);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
        model = glm::scale(model, box_scale);
        sc.setMat4("model", glm::value_ptr(model));

        mat.FresnelR0 = glm::vec3(0.05f);
        mat.Roughness = 0.3f;
        mat.DiffuseAlbedo = glm::vec4(1.0f);
        sc.setVec4("gMat.DiffuseAlbedo", glm::value_ptr(mat.DiffuseAlbedo));
        sc.setVec3("gMat.FresnelR0", glm::value_ptr(mat.FresnelR0));
        sc.setFloat("gMat.Roughness", mat.Roughness);
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, int(skull.indices.size()), GL_UNSIGNED_INT, 0);
        
        //
        // 2.mirror(stencil)
        //
        // mirror plane function
        glm::vec3 N = glm::vec3(0.f, 0.f, 1.f);
        glm::vec3 P0 = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 mirror_trans = P0;
        glm::vec3 mirror_scale = glm::vec3(12.0f, 12.0f, 0.01f);
        
        glm::mat4 M = glm::mat4(1.0);
        glm::vec3 m1 = glm::vec3(2 * glm::dot(N, P0));
        M = glm::translate(M, m1);
        glm::vec3 m2 = glm::vec3(1.f) - 2.f * N;
        M = glm::scale(M, m2);
        
        glDepthMask(0x00);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        model = glm::mat4(1.0);
        model = glm::translate(model, mirror_trans);
        model = glm::scale(model, mirror_scale);
        sc.setMat4("model", glm::value_ptr(model));
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, int(box.indices.size()), GL_UNSIGNED_INT, 0);
        glDepthMask(0xFF);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        
        
        //
        // 3.box(in mirror)
        //
        glCullFace(GL_FRONT);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // light reflect
        auto ld1 = glm::mat3(M) * lights[0].Direction;
        auto ld2 = glm::mat3(M) * lights[1].Direction;
        auto ld3 = glm::mat3(M) * lights[2].Direction;
        sc.setVec3("gLights[0].Direction", glm::value_ptr(ld1));
        sc.setVec3("gLights[1].Direction", glm::value_ptr(ld2));
        sc.setVec3("gLights[2].Direction", glm::value_ptr(ld3));
        model = glm::mat4(1.0);
        model = model * M;
        model = glm::translate(model, box_trans);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
        model = glm::scale(model, box_scale);
        sc.setMat4("model", glm::value_ptr(model));
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, int(skull.indices.size()), GL_UNSIGNED_INT, 0);
        glDisable(GL_STENCIL_TEST);
        
        
        //
        // 4.mirror
        //
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glDepthMask(0x00);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        model = glm::mat4(1.0);
        model = glm::translate(model, mirror_trans);
        model = glm::scale(model, mirror_scale);
        sc.setMat4("model", glm::value_ptr(model));
        mat.FresnelR0 = glm::vec3(0.05f);
        mat.Roughness = 0.3f;
        mat.DiffuseAlbedo = glm::vec4(0.1f, 0.1f, 0.1f, 0.2f);
        sc.setVec4("gMat.DiffuseAlbedo", glm::value_ptr(mat.DiffuseAlbedo));
        sc.setVec3("gMat.FresnelR0", glm::value_ptr(mat.FresnelR0));
        sc.setFloat("gMat.Roughness", mat.Roughness);
        sc.setVec3("gLights[0].Direction", glm::value_ptr(lights[0].Direction));
        sc.setVec3("gLights[1].Direction", glm::value_ptr(lights[1].Direction));
        sc.setVec3("gLights[2].Direction", glm::value_ptr(lights[2].Direction));
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, int(box.indices.size()), GL_UNSIGNED_INT, 0);
        glDepthMask(0xFF);
        
        ig.DrawWindow();
        glfwSwapBuffers(window);
    }
    
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(2, EBO);
    glDeleteVertexArrays(2, VAO);
    sc.destory();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action == GLFW_PRESS)
        {
            leftMouseButtonPressed = true;
        }
        else if(action == GLFW_RELEASE)
        {
            leftMouseButtonPressed = false;
        }
    }
    
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        mouseOnImguiWindow = true;
    else
        mouseOnImguiWindow = false;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    
    if(leftMouseButtonPressed && !mouseOnImguiWindow){
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
