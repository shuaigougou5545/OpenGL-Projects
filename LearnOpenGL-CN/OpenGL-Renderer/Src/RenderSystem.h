#ifndef RenderSystem_h
#define RenderSystem_h


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "DebugFunction.h"
#include "ShaderConstructor.h"
#include "LoadTexture.h"
#include "Model.h"
#include "Light.h"
#include "Material.h"

//#include "WindowSystem.h"


namespace rd {

class RenderSystem{
public:
    RenderSystem(WindowSystem* _ws) : window_sys(_ws){}
    ~RenderSystem() = default;
    
    void initialize();
    void run();
    void shutdown();
    
    void initModels();
    void initOpenGLObjects();
    void initTextures();
    void initOpenGLStatus();
    void initShaders();
private:
    std::vector<Model> models;
    std::vector<GLuint> VBOs;
    std::vector<GLuint> EBOs;
    std::vector<GLuint> VAOs;
    std::vector<GLuint> textures;
    std::vector<ShaderConstructor> shader_constructors;
    WindowSystem* window_sys;
};

}

#endif /* RenderSystem_h */
