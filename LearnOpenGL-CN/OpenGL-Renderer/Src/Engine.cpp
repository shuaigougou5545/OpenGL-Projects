#include "Engine.h"

#include <thread>


Engine::Engine()
{
    
}

Engine::~Engine()
{
    
}

void Engine::startEngine()
{
    WindowInfo info;
    
    window_sys = std::make_shared<WindowSystem>();
    window_sys->initialize(info);

    render_sys = std::make_shared<RenderSystem>(window_sys);
    render_sys->initialize();
    
    window = window_sys->getWindow();
}

void Engine::run()
{
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        render_sys->tick(deltaTime);
        window_sys->tick(deltaTime);
        
        glfwSwapBuffers(window);
    }
}

void Engine::shutdownEngine()
{
    window_sys->shutdown();
    render_sys->shutdown();
}
