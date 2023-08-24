#ifndef Light_h
#define Light_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Light{
    glm::vec3 Strength;
    float FalloffStart; // point/spot light only
    glm::vec3 Position; // point light only
    float FalloffEnd; // point/spot light only
    glm::vec3 Direction; // directional/spot light only
    float SpotPower; // spot light only
};


#endif /* Light_h */
