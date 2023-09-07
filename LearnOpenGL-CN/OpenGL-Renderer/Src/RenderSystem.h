#ifndef RenderSystem_h
#define RenderSystem_h


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <memory>

#include "DebugFunction.h"
#include "LoadTexture.h"
#include "ShaderConstructor.h"
#include "Model.h"
#include "Light.h"
#include "Material.h"
#include "WindowSystem.h"
#include "PostProcess.h"


struct MVP{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
};


class RenderSystem{
public:
    RenderSystem(WindowSystem* _ws) : window_sys(_ws){}
    ~RenderSystem() = default;
    
    void initialize();
    void tick(float delta_time);
    void shutdown();
    
    void initModels();
    void initOpenGLObjects();
    void initTextures();
    void initShaders();
    void initLogic();
private:
    std::vector<Model> models;
    std::vector<GLuint> VBOs;
    std::vector<GLuint> EBOs;
    std::vector<GLuint> VAOs;
    std::vector<GLuint> textures;
    std::vector<ShaderConstructor> shader_constructors;
    WindowSystem* window_sys;
    int window_width = 800, window_height = 600;
    int viewport_width = 1600, viewport_height = 1200;
    
    std::shared_ptr<PostProcess> postprocess_ptr;
};


#endif /* RenderSystem_h */
