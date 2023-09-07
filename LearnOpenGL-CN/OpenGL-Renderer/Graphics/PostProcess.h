#ifndef PostProcess_h
#define PostProcess_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>

#include "ShaderConstructor.h"


class PostProcess{
public:
    PostProcess(int width, int height);
    ~PostProcess();
    
    void BindFBO();
    void RenderToTexture();
    void Destroy();
    
    GLuint fbo;
    GLuint texture;
    GLuint rbo;
    
    GLuint quadVAO;
    GLuint quadVBO;
    
    std::shared_ptr<ShaderConstructor> shader_constructor_ptr;
    
private:
    void CreateOpenGLObjects();
    void CreateShader();
    void DrawScreenQuad();
    
    int m_width, m_height;
    std::vector<float> quad_vertices = {
    //  --- pos ---
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f,
    };
};

#endif /* PostProcess_h */
