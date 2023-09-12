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
#include "Skybox.h"
#include "NormalVisualization.h"


struct cbPass{
    // binding point: 0
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
};


class RenderSystem{
public:
    RenderSystem(std::shared_ptr<WindowSystem> _ws) : window_sys(_ws){}
    ~RenderSystem();
    
    void initialize();
    void tick(float delta_time);
    void shutdown();
    
    void initModels();
    void initOpenGLObjects();
    void initUniformBlocks();
    void initTextures();
    void initShaders();
    void initLogic();
    void initSkybox();
    void initNormalVisualization();
    
    void updateLogic();
    void draw();
    void updateUniformBlocks();
    void drawSkybox();
    void drawNormalVisualization();
    
private:
    std::vector<Model> models;
    std::vector<GLuint> VBOs;
    std::vector<GLuint> EBOs;
    std::vector<GLuint> VAOs;
    std::vector<GLuint> textures;
    
    std::shared_ptr<WindowSystem> window_sys;
    Camera* camera;
    int window_width = 800, window_height = 600;
    int viewport_width = 1600, viewport_height = 1200;
    
    std::shared_ptr<ShaderConstructor> shader_constructor_ptr;
    std::shared_ptr<PostProcess> postprocess_ptr;
    std::shared_ptr<Skybox> skybox_ptr;
    std::shared_ptr<NormalVisualization> normalvisualization_ptr;
    
    GLuint ubo_cbPass;
};


#endif /* RenderSystem_h */
