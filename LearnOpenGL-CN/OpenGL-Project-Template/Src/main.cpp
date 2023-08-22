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
#include "ShapeGenerator.h"
#include "LoadTexture.h"
#include "ImguiGenerator.h"

#include "Material.h"
#include "Light.h"


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.f, 0.f, 3.f));

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
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    
    //
    // vertex input:
    //
    Box box;
    
    
    //
    // VAO VBO EBO
    //
    unsigned int VAO, VBO, EBO;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    glBufferData(GL_ARRAY_BUFFER, box.vertices.size() * sizeof(float), box.vertices.data(), GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, box.indices.size() * sizeof(unsigned int), box.indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    
    //
    // texture
    //
    unsigned int texture;
    texture = loadTexture("./Textures/container2.png");
    
    
    //
    // depth
    //
    glEnable(GL_DEPTH_TEST);
    
    
    //
    // shader
    //
    ShaderConstructor sc("./Shaders/VS.vert", "./Shaders/FS.frag", "", "");
    
    
    //
    // other
    //
    sc.use();
    sc.setInt("mat.diffuse", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    
    
    
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        glfwPollEvents();
        
        
        glm::mat4 model, view, projection;
        model = view = projection = glm::mat4(1.0);
        glm::vec3 trans = glm::vec3(0.f, 0.f, -5.f);
        float rotate_angle = 0.0f;
        glm::vec3 rotate_axis = glm::vec3(0.5f, 0.5f, 0.f);
        glm::vec3 scale = glm::vec3(1.f);
        
        Light light;
        light.position = glm::vec3(1.2f, 1.0f, 2.0f);
        
        Material mat;
        
        
        // static imgui variable
        static bool im_button_light_model = false;
        static bool im_buttom_material_model = false;
        static glm::vec3 im_light_color = glm::vec3(1.f);
        static glm::vec3 im_ambient_light = glm::vec3(0.1f);
        static glm::vec3 im_diffuse_albedo = glm::vec3(1.f, 0.5f, 0.31f);
        static glm::vec3 im_fresnel_r0 = glm::vec3(0.2f);
        static float im_roughness = 0.9f;
        
        
        ig.InitWindow();
        
        ImGui::Begin("OpenGL");
        
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Light Position: (%.1f, %.1f, %.1f)", light.position.x, light.position.y, light.position.z);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Checkbox("Light Model Info", &im_button_light_model);
        ImGui::Checkbox("Material Model Info", &im_buttom_material_model);
        
        int window_width = 0, window_height = 0;
        glfwGetWindowSize(window, &window_width, &window_height);
        ImGui::Text("Framebuffer: width - %i, height - %i", window_width, window_height);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ig.io->Framerate, ig.io->Framerate);
        
        ImGui::End();
        
        if(im_button_light_model)
        {
            ImGui::Begin("Light Model Info");
            ImGui::ColorEdit3("Light Color", glm::value_ptr(im_light_color));
            ImGui::ColorEdit3("Ambient Color", glm::value_ptr(im_ambient_light));
            ImGui::End();
        }
        
        if(im_buttom_material_model)
        {
            ImGui::Begin("Material Model Info");
            ImGui::ColorEdit3("Diffuse Albedo", glm::value_ptr(im_diffuse_albedo));
            ImGui::ColorEdit3("Fresnel R0", glm::value_ptr(im_fresnel_r0));
            ImGui::SliderFloat("Roughness", &im_roughness, 0.0, 1.0);
            ImGui::End();
        }
        
        ImGui::Render();
        
        
        //
        // Box
        //
        model = glm::translate(model, trans);
        model = glm::rotate(model, glm::radians(rotate_angle), rotate_axis);
        model = glm::scale(model, scale);
        
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
    
        sc.setMat4("model", glm::value_ptr(model));
        sc.setMat4("view", glm::value_ptr(view));
        sc.setMat4("projection", glm::value_ptr(projection));
        
        light.color = im_light_color;

        sc.setVec3("light0.position", glm::value_ptr(light.position));
        sc.setVec3("light0.color", glm::value_ptr(light.color));
        
        glm::vec3 ambientLight = im_ambient_light;
        sc.setVec3("ambientLight", glm::value_ptr(ambientLight));
        
        mat.diffuseAlbedo = im_diffuse_albedo;
        mat.fresnelR0 = im_fresnel_r0;
        mat.roughness = im_roughness;

        sc.setVec3("mat0.diffuseAlbedo", glm::value_ptr(mat.diffuseAlbedo));
        sc.setVec3("mat0.fresnelR0", glm::value_ptr(mat.fresnelR0));
        sc.setFloat("mat0.roughness", mat.roughness);

        sc.setVec3("viewPos", glm::value_ptr(camera.Position));
        
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //
        // Light Box
        //
//        trans = light.position;
//        scale = glm::vec3(0.1f);
//        model = glm::translate(model, trans);
//        model = glm::scale(model, scale);
//        mat.diffuseAlbedo = glm::vec3(1.0);
//        mat.fresnelR0 = glm::vec3(1.0);
//        mat.roughness = 1.0f;
//        sc.setMat4("model", glm::value_ptr(model));
//        sc.setVec3("mat0.diffuseAlbedo", glm::value_ptr(mat.diffuseAlbedo));
//        sc.setVec3("mat0.fresnelR0", glm::value_ptr(mat.fresnelR0));
//        sc.setFloat("mat0.roughness", mat.roughness);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        
        ig.DrawWindow();
        
        glfwSwapBuffers(window);
    }
    
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
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
