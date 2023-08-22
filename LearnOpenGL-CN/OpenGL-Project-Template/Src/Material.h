#ifndef Material_h
#define Material_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Material{
    glm::vec4 DiffuseAlbedo;
    glm::vec3 FresnelR0;
    float Roughness;
};


#endif /* Material_h */
