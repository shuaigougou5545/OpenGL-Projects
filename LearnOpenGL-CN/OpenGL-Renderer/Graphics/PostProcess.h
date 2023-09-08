#ifndef PostProcess_h
#define PostProcess_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>

#include "ShaderConstructor.h"



struct GuassianBlurInfo{
    float blur_size = 1.0f;
};

struct PostProcessInfo{
    bool isUsePostProcessing = false;
    bool isUseGaussianBlur = false;
    GuassianBlurInfo guassian_blur_info;
};

class PostProcess{
public:
    PostProcess(int width, int height, PostProcessInfo info = PostProcessInfo());
    ~PostProcess();
    
    PostProcessInfo m_info;
    
    void Initialize();
    void Destroy();
    
    void BindFBO();
    void RenderToTexture();
    
    std::shared_ptr<ShaderConstructor> shader_constructor_ptr;
    
    void GaussianBlur();
    
private:
    GLuint fbo;
    GLuint texture;
    GLuint rbo;
    
    GLuint quadVAO;
    GLuint quadVBO;
    
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
