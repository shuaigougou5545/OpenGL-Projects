#ifndef PostProcess_h
#define PostProcess_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "ShaderConstructor.h"


class PostProcess{
public:
    PostProcess(int width, int height);
    ~PostProcess();
    
    void BindFBO();
    void RenderToTexture();
    
    GLuint fbo;
    GLuint texture;
    GLuint rbo;
    
    GLuint quadVAO;
    GLuint quadVBO;
    GLuint quadEBO;
    
    std::vector<ShaderConstructor> scLists;
    
private:
    void CreateOpenGLObjects();
    void CreateShader();
    void DrawQuad();
    
    int m_width, m_height;
    std::vector<float> quad_vertices = {
    //  --- pos ---  --- uv ---
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f,
       -1.0f,  1.0f, 0.0f, 1.0f,
       -1.0f, -1.0f, 0.0f, 0.0f,
    };
    std::vector<unsigned int> quad_indices = {
        0, 1, 2,
        1, 2, 3,
    };
};

#endif /* PostProcess_h */
