#ifndef Material_h
#define Material_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Material{
    glm::vec3 diffuseAlbedo;
    glm::vec3 fresnelR0;
    float roughness;
};


#endif /* Material_h */
