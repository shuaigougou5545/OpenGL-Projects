#include "PostProcess.h"
#include "DebugFunction.h"


PostProcess::PostProcess(int width, int height, PostProcessInfo info) : m_width(width), m_height(height), m_info(info)
{
    CreateOpenGLObjects();
}

PostProcess::~PostProcess()
{
    
}

void PostProcess::CreateOpenGLObjects()
{
    // FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR [framebuffer]: framebuffer is not complete" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // VAO VBO EBO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, quad_vertices.size() * sizeof(float), quad_vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PostProcess::CreateShader()
{
    std::string vsMacroString = "";
    std::string fsMacroString = "";
    
    if(m_info.isUseGaussianBlur)
        fsMacroString += "#define GAUSSIAN_BLUR\n";
    
    shader_constructor_ptr = std::make_shared<ShaderConstructor>("./Shaders/PostProcess_vs.vert", "./Shaders/PostProcess_fs.frag", vsMacroString, fsMacroString);
}

void PostProcess::BindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void PostProcess::RenderToTexture()
{
    CreateShader();
    shader_constructor_ptr->use();
    
    if(m_info.isUseGaussianBlur)
        GaussianBlur();
    
    DrawScreenQuad();
}

void PostProcess::GaussianBlur()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    shader_constructor_ptr->setInt("source", 0);
    glm::vec2 pixel_size = glm::vec2(1.0f/m_width, 1.0f/m_height);
    shader_constructor_ptr->setVec2("pixel_size", glm::value_ptr(pixel_size));
    float blur_size = 5.0f;
    shader_constructor_ptr->setFloat("blur_size", blur_size);
}

void PostProcess::DrawScreenQuad()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcess::Destroy()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}
