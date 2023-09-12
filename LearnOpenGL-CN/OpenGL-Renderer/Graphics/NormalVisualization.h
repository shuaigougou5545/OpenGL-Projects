#ifndef NormalVisualization_h
#define NormalVisualization_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>

#include "ShaderConstructor.h"


class NormalVisualization{

public:
    NormalVisualization();
    ~NormalVisualization();
    
    void initShaders();
    void use_shader(float normal_len);
    
    std::shared_ptr<ShaderConstructor> sc_ptr;
private:
    
};

#endif /* NormalVisualization_h */
