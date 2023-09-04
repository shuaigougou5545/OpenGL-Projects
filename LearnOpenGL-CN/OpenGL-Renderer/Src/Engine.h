#ifndef GraphicsEngine_h
#define GraphicsEngine_h

#include "RenderSystem.h"


namespace rd{

class Engine{
public:
    Engine();
    ~Engine();
    
    void startEngine();
    void run();
    void shutdownEngine();
    
    
private:
//    GLFWwindow* window;
//    std::shared_ptr<WindowSystem> window_sys;
//    std::shared_ptr<RenderSystem> render_sys;
//    std::shared_ptr<Camera> camera;
    
    // time
    float deltaTime = 0.f;
    float lastFrame = 0.f;
};

}

#endif /* GraphicsEngine_h */
